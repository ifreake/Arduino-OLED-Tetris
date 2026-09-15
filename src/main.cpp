#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

// =====================================================
// BUTTONS
// =====================================================

#define BUTTON_LEFT   2
#define BUTTON_RIGHT  3
#define BUTTON_DOWN   4
#define BUTTON_ROTATE 5

// =====================================================
// TETRIS BOARD
// =====================================================

#define BOARD_WIDTH  10
#define BOARD_HEIGHT 20

// اندازه هر خانه
#define CELL_SIZE 3

// محل زمین بازی روی OLED
#define BOARD_X 48
#define BOARD_Y 2

// =====================================================
// GAME BOARD
// =====================================================

bool board[BOARD_HEIGHT][BOARD_WIDTH];

// =====================================================
// PIECES
// =====================================================

struct Point
{
    int8_t x;
    int8_t y;
};

struct Piece
{
    byte type;
    byte rotation;

    int x;
    int y;
};

Piece currentPiece;

// =====================================================
// PIECE DATA
// =====================================================

// I
const Point PIECE_I[4][4] =
{
    {
        {0,1},
        {1,1},
        {2,1},
        {3,1}
    },

    {
        {2,0},
        {2,1},
        {2,2},
        {2,3}
    },

    {
        {0,2},
        {1,2},
        {2,2},
        {3,2}
    },

    {
        {1,0},
        {1,1},
        {1,2},
        {1,3}
    }
};

// O
const Point PIECE_O[1][4] =
{
    {
        {0,0},
        {1,0},
        {0,1},
        {1,1}
    }
};

// T
const Point PIECE_T[4][4] =
{
    {
        {1,0},
        {0,1},
        {1,1},
        {2,1}
    },

    {
        {1,0},
        {1,1},
        {2,1},
        {1,2}
    },

    {
        {0,1},
        {1,1},
        {2,1},
        {1,2}
    },

    {
        {1,0},
        {0,1},
        {1,1},
        {1,2}
    }
};

// L
const Point PIECE_L[4][4] =
{
    {
        {2,0},
        {0,1},
        {1,1},
        {2,1}
    },

    {
        {1,0},
        {1,1},
        {1,2},
        {2,2}
    },

    {
        {0,1},
        {1,1},
        {2,1},
        {0,2}
    },

    {
        {0,0},
        {1,0},
        {1,1},
        {1,2}
    }
};

// J
const Point PIECE_J[4][4] =
{
    {
        {0,0},
        {0,1},
        {1,1},
        {2,1}
    },

    {
        {1,0},
        {2,0},
        {1,1},
        {1,2}
    },

    {
        {0,1},
        {1,1},
        {2,1},
        {2,2}
    },

    {
        {1,0},
        {1,1},
        {0,2},
        {1,2}
    }
};

// S
const Point PIECE_S[2][4] =
{
    {
        {1,0},
        {2,0},
        {0,1},
        {1,1}
    },

    {
        {0,0},
        {0,1},
        {1,1},
        {1,2}
    }
};

// Z
const Point PIECE_Z[2][4] =
{
    {
        {0,0},
        {1,0},
        {1,1},
        {2,1}
    },

    {
        {1,0},
        {0,1},
        {1,1},
        {0,2}
    }
};

// =====================================================
// GAME VARIABLES
// =====================================================

bool gameRunning = false;
bool gameOver = false;

unsigned long lastFallTime = 0;

unsigned long fallDelay = 500;

const unsigned long fastFallDelay = 70;

unsigned long lastMoveTime = 0;

const unsigned long moveDelay = 120;

bool previousRotateState = HIGH;

// =====================================================
// GET PIECE POINT
// =====================================================

Point getPiecePoint(
    byte type,
    byte rotation,
    byte index
)
{
    switch (type)
    {
        case 0:
            return PIECE_I[rotation][index];

        case 1:
            return PIECE_O[0][index];

        case 2:
            return PIECE_T[rotation][index];

        case 3:
            return PIECE_L[rotation][index];

        case 4:
            return PIECE_J[rotation][index];

        case 5:
            return PIECE_S[rotation][index];

        default:
            return PIECE_Z[rotation][index];
    }
}

// =====================================================
// ROTATION COUNT
// =====================================================

byte getRotationCount(byte type)
{
    switch (type)
    {
        case 0:
            return 4;

        case 1:
            return 1;

        case 2:
            return 4;

        case 3:
            return 4;

        case 4:
            return 4;

        case 5:
            return 2;

        case 6:
            return 2;
    }

    return 1;
}

// =====================================================
// COLLISION
// =====================================================

bool checkCollision(
    int newX,
    int newY,
    byte newRotation
)
{
    for (byte i = 0; i < 4; i++)
    {
        Point p = getPiecePoint(
            currentPiece.type,
            newRotation,
            i
        );

        int x = newX + p.x;
        int y = newY + p.y;

        // چپ و راست
        if (x < 0 || x >= BOARD_WIDTH)
            return true;

        // پایین
        if (y >= BOARD_HEIGHT)
            return true;

        // برخورد با بلوک‌های قبلی
        if (y >= 0 && board[y][x])
            return true;
    }

    return false;
}

// =====================================================
// CREATE NEW PIECE
// =====================================================

void createNewPiece()
{
    currentPiece.type = random(0, 7);

    currentPiece.rotation = 0;

    currentPiece.x = 3;

    currentPiece.y = 0;

    if (checkCollision(
            currentPiece.x,
            currentPiece.y,
            currentPiece.rotation
        ))
    {
        gameOver = true;
        gameRunning = false;
    }
}

// =====================================================
// LOCK PIECE
// =====================================================

void lockPiece()
{
    for (byte i = 0; i < 4; i++)
    {
        Point p = getPiecePoint(
            currentPiece.type,
            currentPiece.rotation,
            i
        );

        int x = currentPiece.x + p.x;
        int y = currentPiece.y + p.y;

        if (
            x >= 0 &&
            x < BOARD_WIDTH &&
            y >= 0 &&
            y < BOARD_HEIGHT
        )
        {
            board[y][x] = true;
        }
    }
}

// =====================================================
// CLEAR LINES
// =====================================================

void clearLines()
{
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--)
    {
        bool fullLine = true;

        for (byte x = 0; x < BOARD_WIDTH; x++)
        {
            if (!board[y][x])
            {
                fullLine = false;
                break;
            }
        }

        if (fullLine)
        {
            // همه ردیف‌ها را یک خانه پایین ببر
            for (int yy = y; yy > 0; yy--)
            {
                for (byte x = 0; x < BOARD_WIDTH; x++)
                {
                    board[yy][x] =
                        board[yy - 1][x];
                }
            }

            // ردیف بالا خالی
            for (byte x = 0; x < BOARD_WIDTH; x++)
            {
                board[0][x] = false;
            }

            y++;
        }
    }
}

// =====================================================
// MOVE LEFT
// =====================================================

void moveLeft()
{
    if (!checkCollision(
            currentPiece.x - 1,
            currentPiece.y,
            currentPiece.rotation
        ))
    {
        currentPiece.x--;
    }
}

// =====================================================
// MOVE RIGHT
// =====================================================

void moveRight()
{
    if (!checkCollision(
            currentPiece.x + 1,
            currentPiece.y,
            currentPiece.rotation
        ))
    {
        currentPiece.x++;
    }
}

// =====================================================
// MOVE DOWN
// =====================================================

bool moveDown()
{
    if (!checkCollision(
            currentPiece.x,
            currentPiece.y + 1,
            currentPiece.rotation
        ))
    {
        currentPiece.y++;
        return true;
    }

    return false;
}

// =====================================================
// ROTATE
// =====================================================

void rotatePiece()
{
    byte count =
        getRotationCount(currentPiece.type);

    if (count == 1)
        return;

    byte nextRotation =
        (currentPiece.rotation + 1) % count;

    // چرخش عادی
    if (!checkCollision(
            currentPiece.x,
            currentPiece.y,
            nextRotation
        ))
    {
        currentPiece.rotation =
            nextRotation;

        return;
    }

    // Wall kick چپ
    if (!checkCollision(
            currentPiece.x - 1,
            currentPiece.y,
            nextRotation
        ))
    {
        currentPiece.x--;

        currentPiece.rotation =
            nextRotation;

        return;
    }

    // Wall kick راست
    if (!checkCollision(
            currentPiece.x + 1,
            currentPiece.y,
            nextRotation
        ))
    {
        currentPiece.x++;

        currentPiece.rotation =
            nextRotation;
    }
}

// =====================================================
// DRAW CELL
// =====================================================

void drawCell(
    int x,
    int y
)
{
    int px =
        BOARD_X + x * CELL_SIZE;

    int py =
        BOARD_Y + y * CELL_SIZE;

    display.fillRect(
        px,
        py,
        CELL_SIZE - 1,
        CELL_SIZE - 1,
        SSD1306_WHITE
    );
}

// =====================================================
// DRAW GAME
// =====================================================

void drawGame()
{
    display.clearDisplay();

    // کادر بازی
    display.drawRect(
        BOARD_X - 2,
        BOARD_Y - 2,
        BOARD_WIDTH * CELL_SIZE + 4,
        BOARD_HEIGHT * CELL_SIZE + 4,
        SSD1306_WHITE
    );

    // بلوک‌های ثابت
    for (byte y = 0; y < BOARD_HEIGHT; y++)
    {
        for (byte x = 0; x < BOARD_WIDTH; x++)
        {
            if (board[y][x])
            {
                drawCell(x, y);
            }
        }
    }

    // قطعه فعلی
    if (gameRunning && !gameOver)
    {
        for (byte i = 0; i < 4; i++)
        {
            Point p =
                getPiecePoint(
                    currentPiece.type,
                    currentPiece.rotation,
                    i
                );

            int x =
                currentPiece.x + p.x;

            int y =
                currentPiece.y + p.y;

            if (
                x >= 0 &&
                x < BOARD_WIDTH &&
                y >= 0 &&
                y < BOARD_HEIGHT
            )
            {
                drawCell(x, y);
            }
        }
    }

    display.display();
}

// =====================================================
// START SCREEN
// =====================================================

void showStartScreen()
{
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);

    display.setCursor(30, 5);

    display.println("TETRIS");

    display.setTextSize(1);

    display.setCursor(28, 30);

    display.println("OLED 0.96");

    display.setCursor(19, 48);

    display.println("PRESS ROTATE");

    display.display();
}

// =====================================================
// GAME OVER SCREEN
// =====================================================

void showGameOver()
{
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);

    display.setCursor(20, 8);

    display.println("GAME");

    display.setCursor(20, 30);

    display.println("OVER");

    display.setTextSize(1);

    display.setCursor(18, 52);

    display.println("PRESS ROTATE");

    display.display();
}

// =====================================================
// RESET GAME
// =====================================================

void resetGame()
{
    for (byte y = 0; y < BOARD_HEIGHT; y++)
    {
        for (byte x = 0; x < BOARD_WIDTH; x++)
        {
            board[y][x] = false;
        }
    }

    gameOver = false;

    gameRunning = true;

    fallDelay = 500;

    createNewPiece();

    lastFallTime = millis();
}

// =====================================================
// READ BUTTONS
// =====================================================

void readButtons()
{
    unsigned long now = millis();

    bool leftPressed =
        digitalRead(BUTTON_LEFT) == LOW;

    bool rightPressed =
        digitalRead(BUTTON_RIGHT) == LOW;

    bool downPressed =
        digitalRead(BUTTON_DOWN) == LOW;

    bool rotatePressed =
        digitalRead(BUTTON_ROTATE) == LOW;

    // چپ و راست
    if (now - lastMoveTime >= moveDelay)
    {
        if (leftPressed && !rightPressed)
        {
            moveLeft();

            lastMoveTime = now;
        }
        else if (rightPressed && !leftPressed)
        {
            moveRight();

            lastMoveTime = now;
        }
    }

    // چرخش
    if (
        rotatePressed &&
        previousRotateState == HIGH
    )
    {
        rotatePiece();
    }

    previousRotateState =
        rotatePressed;

    // سقوط سریع
    if (downPressed)
    {
        if (
            now - lastFallTime >=
            fastFallDelay
        )
        {
            lastFallTime = now;

            if (!moveDown())
            {
                lockPiece();

                clearLines();

                createNewPiece();
            }
        }
    }
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
    // کلیدها
    pinMode(
        BUTTON_LEFT,
        INPUT_PULLUP
    );

    pinMode(
        BUTTON_RIGHT,
        INPUT_PULLUP
    );

    pinMode(
        BUTTON_DOWN,
        INPUT_PULLUP
    );

    pinMode(
        BUTTON_ROTATE,
        INPUT_PULLUP
    );

    // Random
    randomSeed(
        analogRead(A0)
    );

    // I2C
    Wire.begin();

    // OLED
    if (
        !display.begin(
            SSD1306_SWITCHCAPVCC,
            OLED_ADDRESS
        )
    )
    {
        while (true)
        {
            delay(100);
        }
    }

    display.clearDisplay();

    display.display();

    showStartScreen();
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
    // اگر بازی شروع نشده
    if (!gameRunning)
    {
        if (gameOver)
        {
            showGameOver();
        }
        else
        {
            showStartScreen();
        }

        // دکمه Rotate برای شروع
        if (
            digitalRead(
                BUTTON_ROTATE
            ) == LOW
        )
        {
            delay(30);

            if (
                digitalRead(
                    BUTTON_ROTATE
                ) == LOW
            )
            {
                resetGame();

                while (
                    digitalRead(
                        BUTTON_ROTATE
                    ) == LOW
                )
                {
                    delay(10);
                }
            }
        }

        return;
    }

    // خواندن کلیدها
    readButtons();

    // سقوط اتوماتیک
    unsigned long now = millis();

    if (
        now - lastFallTime >=
        fallDelay
    )
    {
        lastFallTime = now;

        if (!moveDown())
        {
            lockPiece();

            clearLines();

            createNewPiece();
        }
    }

    // نمایش
    drawGame();
}
