#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>

#define TITLE "TeSnake"
#define LOST_MSG "YOU LOST!!!"
#define PAUSE_MSG "GAME PAUSED"
#define WINDOW_WIDTH 750
#define WINDOW_HEIGHT 540
#define WINDOW_X (1366 - WINDOW_WIDTH) / 2
#define WINDOW_Y (768 - WINDOW_HEIGHT) / 2
#define FIELD_SIZE_X 30
#define FIELD_SIZE_Y 20
#define PIXEL_PER_SQUARE 25
#define FILED_WIDTH FIELD_SIZE_X * PIXEL_PER_SQUARE
#define FILED_HEIGHT FIELD_SIZE_Y * PIXEL_PER_SQUARE
#define SNAKE_X 5
#define SNAKE_Y 10
#define SNAKE_SIZE_MIN 4
#define SNAKE_SIZE_MAX 300
#define TIMER_DELAY 300
#define FRUIT_NUMBER 10
#define POISON_NUMBER 10
#define SCORE_INREMENT 5
#define LIMIT_ITERRATIONS 1000

typedef struct Snake Snake;
typedef struct Node Node;
typedef enum Direction {UP, DOWN, LEFT, RIGHT} Direction;
typedef struct Fruit Fruit;
typedef struct Poison Poison;

struct Node
{
    int x, y;
};

struct Snake
{
    int len, score;
    Node body[SNAKE_SIZE_MAX];
    Node oldTailPos;
    Direction dir;
    bool isAlive;
} snake;

struct Fruit
{
    int x, y;
    bool isEaten;
} fruits[FRUIT_NUMBER];

struct Poison
{
    int x, y;
}poisons[POISON_NUMBER];

bool isGameOver = false;
bool isPaused = false;

void display();
void timer();
void myKeyboardFunc(int key, int x, int y);
void myKeyboardSpecialFunc(int key, int x, int y);
void drawField();
void snake_initialize();
void snake_step();
void snake_turn(Direction dir);
bool is_self_crossing();
void create_random_fruit_setup();
void check_fruits();
void fruits_draw();
void create_random_poison_setup();
void check_poisons();
void poisons_draw();
void snake_draw();
void snake_info_draw();
void drawWord(char *word, float x, float y, float space, int red, int green, int blue);

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(WINDOW_X, WINDOW_Y);
    glutCreateWindow(TITLE);
    //glClearColor(0, 0, 72.0 / 255, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,WINDOW_WIDTH,0,WINDOW_HEIGHT);
    //gluOrtho2D(0, FILED_WIDTH, FILED_HEIGHT, 0);
    glutDisplayFunc(display);
    glutKeyboardFunc(myKeyboardFunc);
    glutSpecialFunc(myKeyboardSpecialFunc);
    glutTimerFunc(TIMER_DELAY, timer, 0);
    snake_initialize();
    create_random_fruit_setup();
    create_random_poison_setup();
    glutMainLoop();
    return 0;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawField();
    fruits_draw();
    poisons_draw();
    snake_draw();
    snake_info_draw();
    glFlush();
}

void timer()
{
    if(!isGameOver && !isPaused)
    {
        if(!is_self_crossing())
        {
            snake_step();
            check_fruits();
            check_poisons();
        }
        else
        {
            isGameOver = true;
        }
    }
    display();
    glutTimerFunc(TIMER_DELAY, timer, 0);
}


void snake_initialize()
{
    snake.len = SNAKE_SIZE_MIN;
    snake.score = 0;
    snake.dir = RIGHT;
    snake.isAlive = true;
    for(int i = 0; i < SNAKE_SIZE_MIN; ++i)
    {
        snake.body[i].x = SNAKE_X - i;
        snake.body[i].y = SNAKE_Y;
    }
    snake.oldTailPos.x = snake.body[snake.len - 1].x - 1;
    snake.oldTailPos.y = SNAKE_Y;
}

void snake_step()
{
    snake.oldTailPos.x = snake.body[snake.len - 1].x;
    snake.oldTailPos.y = snake.body[snake.len - 1].y;
    for(int i = snake.len - 1; i > 0; --i)
    {
        snake.body[i] = snake.body[i - 1];
    }
    switch(snake.dir)
    {
        case UP :
            snake.body[0].y += 1;
            if(snake.body[0].y > FIELD_SIZE_Y - 1)
            {
                snake.body[0].y = 0;
            }
            break;
        case DOWN:
            snake.body[0].y -= 1;
            if(snake.body[0].y < 0)
            {
                snake.body[0].y = FIELD_SIZE_Y - 1;
            }
            break;
        case LEFT :
            snake.body[0].x -= 1;
            if(snake.body[0].x < 0)
            {
                snake.body[0].x = FIELD_SIZE_X - 1;
            }
            break;
        case RIGHT:
            snake.body[0].x += 1;
            if(snake.body[0].x > FIELD_SIZE_X - 1)
            {
                snake.body[0].x = 0;
            }
            break;
        default:
            break;
    }
}

void snake_turn(Direction dir)
{
    if(!((snake.dir == LEFT && dir == RIGHT) ||
       (snake.dir == RIGHT && dir == LEFT) ||
       (snake.dir == UP && dir == DOWN) ||
       (snake.dir == DOWN && dir == UP)))
    {
        snake.dir = dir;
    }
}

bool is_self_crossing()
{
    int head_x = snake.body[0].x;
    int head_y = snake.body[0].y;
    for(int i = 1; i < snake.len; ++i)
    {
        if(head_x == snake.body[i].x && head_y == snake.body[i].y)
        {
            return true;
        }
    }
    return false;
}

void create_random_fruit_setup()
{
    srand(time(NULL));
    for(int i = 0; i < FRUIT_NUMBER; ++i)
    {
        bool isCollisoinsWithOtherFruits = false;
        bool isCollisoinsWithSnake = false;
        int rand_x, rand_y;
        do
        {
            rand_x = rand() % FIELD_SIZE_X;
            rand_y = rand() % FIELD_SIZE_Y;
            for(int j = 0; j < i && !isCollisoinsWithOtherFruits; ++j)
            {
                if(fruits[j].x == rand_x && fruits[j].y == rand_y)
                {
                    isCollisoinsWithOtherFruits = true;
                }
            }
            for(int k = 0; k < snake.len && !isCollisoinsWithOtherFruits && !isCollisoinsWithSnake; ++k)
            {
                if(snake.body[k].x == rand_x && snake.body[k].y == rand_y)
                {
                     isCollisoinsWithSnake = true;
                }
            }
        }while(isCollisoinsWithOtherFruits || isCollisoinsWithSnake);
        fruits[i].x = rand_x;
        fruits[i].y = rand_y;
        fruits[i].isEaten = false;
    }
}

void check_fruits()
{
    for(int i = 0; i < FRUIT_NUMBER; ++i)
    {
        if(snake.body[0].x == fruits[i].x &&
           snake.body[0].y == fruits[i].y)
        {
            //printf("The fruit has been eaten\n");
            snake.score += SCORE_INREMENT;
            ++snake.len;
            snake.body[snake.len - 1].x = snake.oldTailPos.x;
            snake.body[snake.len - 1].y = snake.oldTailPos.y;
            bool isCollisoinsWithOtherFruits = false;
            bool isCollisoinsWithSnake = false;
            int rand_x, rand_y;
            int counter = 0;
            do
            {
                rand_x = rand() % FIELD_SIZE_X;
                rand_y = rand() % FIELD_SIZE_Y;
                for(int j = 0; j < FRUIT_NUMBER && !isCollisoinsWithOtherFruits; ++j)
                {
                    if(i == j) continue;
                    if(fruits[j].x == rand_x && fruits[j].y == rand_y)
                    {
                        isCollisoinsWithOtherFruits = true;
                    }
                }
                for(int k = 0; k < snake.len && !isCollisoinsWithOtherFruits && !isCollisoinsWithSnake; ++k)
                {
                    if(snake.body[k].x == rand_x && snake.body[k].y == rand_y)
                    {
                         isCollisoinsWithSnake = true;
                    }
                }
            }while(++counter < LIMIT_ITERRATIONS && (isCollisoinsWithOtherFruits || isCollisoinsWithSnake));
            fruits[i].x = rand_x;
            fruits[i].y = rand_y;
            break;
        }
    }
}

void create_random_poison_setup()
{
    srand(time(NULL));
    bool isCollisionsWithOtherPoisons;
    bool isCollisionsWithFruits;
    bool isCollisionsWithSnake;
    int rand_x, rand_y;
    for(int i = 0; i < POISON_NUMBER; ++i)
    {
        do
        {
            rand_x = rand() % FIELD_SIZE_X;
            rand_y = rand() % FIELD_SIZE_Y;
            isCollisionsWithOtherPoisons = false;
            isCollisionsWithFruits = false;
            isCollisionsWithSnake = false;
            for(int j = 0; j < i && !isCollisionsWithOtherPoisons; ++j)
            {
                if(rand_x == poisons[j].x && rand_y == poisons[j].y)
                {
                    isCollisionsWithOtherPoisons = true;
                }
            }
            for(int k = 0; k < FRUIT_NUMBER && !isCollisionsWithOtherPoisons && !isCollisionsWithFruits; ++k)
            {
                if(rand_x == fruits[k].x && rand_y == fruits[k].y)
                {
                     isCollisionsWithFruits = true;
                }
            }

            for(int l = 0; l < snake.len && !isCollisionsWithOtherPoisons && !isCollisionsWithFruits && !isCollisionsWithSnake; ++l)
            {
                if(rand_x == snake.body[l].x && rand_y == snake.body[l].y)
                {
                     isCollisionsWithSnake = true;
                }
            }
        }while(isCollisionsWithOtherPoisons || isCollisionsWithFruits || isCollisionsWithSnake);
        poisons[i].x = rand_x;
        poisons[i].y = rand_y;
    }
}

void check_poisons()
{
    for(int i = 0; i < POISON_NUMBER; ++i)
    {
        if(snake.body[0].x == poisons[i].x &&
           snake.body[0].y == poisons[i].y)
        {
            isGameOver = true;
            break;
        }
    }
}

void poisons_draw()
{
    for(int i = 0; i < POISON_NUMBER; ++i)
    {
        glColor3f(1.0, 0.0, 0.0);
        glRectf(poisons[i].x * PIXEL_PER_SQUARE,
                poisons[i].y * PIXEL_PER_SQUARE,
                (poisons[i].x + 1) * PIXEL_PER_SQUARE,
                (poisons[i].y + 1) * PIXEL_PER_SQUARE);
    }
}

void myKeyboardFunc(int key, int x, int y)
{
    switch(key)
    {
        case 'w':
            snake_turn(UP);
            break;
        case 's':
            snake_turn(DOWN);
            break;
            break;
        case 'a':
            snake_turn(LEFT);
            break;
        case 'd':
            snake_turn(RIGHT);
            break;
        case 'n':
            //Starts new game
            snake_initialize();
            create_random_fruit_setup();
            create_random_poison_setup();
            isGameOver = false;
            isPaused = false;
            break;
        case 'p':
            //Pause game
            isPaused = !isPaused;
            break;
        default:
            break;
    }
}

void myKeyboardSpecialFunc(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            snake_turn(UP);
            break;
        case GLUT_KEY_DOWN:
            snake_turn(DOWN);
            break;
            break;
        case GLUT_KEY_LEFT:
            snake_turn(LEFT);
            break;
        case GLUT_KEY_RIGHT:
            snake_turn(RIGHT);
            break;
        default:
            break;
    }
}
void drawField()
{
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    for(int i = 0; i < FILED_WIDTH; i += PIXEL_PER_SQUARE)
    {
        glVertex2f(i, 0);
        glVertex2f(i, FILED_HEIGHT);
    }
    for(int i = 0; i <= FILED_HEIGHT; i += PIXEL_PER_SQUARE)
    {
        glVertex2f(0, i);
        glVertex2f(FILED_WIDTH, i);
    }
    glEnd();
}


void snake_draw()
{
    for(int i = 0; i < snake.len; ++i)
    {
        glColor3f(0.0, 0.0, 1.0);
        glRectf(snake.body[i].x * PIXEL_PER_SQUARE,
                snake.body[i].y * PIXEL_PER_SQUARE,
                (snake.body[i].x + 1) * PIXEL_PER_SQUARE,
                (snake.body[i].y + 1) * PIXEL_PER_SQUARE);

        glBegin(GL_LINES);
        glColor3f(0.0f,0.0f,0.0f);
        int x_a = snake.body[i].x * PIXEL_PER_SQUARE;
        int y_a = snake.body[i].y * PIXEL_PER_SQUARE;
        glVertex2f(x_a, y_a);
        glVertex2f(x_a + PIXEL_PER_SQUARE, y_a);

        glVertex2f(x_a + PIXEL_PER_SQUARE, y_a);
        glVertex2f(x_a + PIXEL_PER_SQUARE, y_a + PIXEL_PER_SQUARE);

        glVertex2f(x_a + PIXEL_PER_SQUARE, y_a + PIXEL_PER_SQUARE);
        glVertex2f(x_a, y_a + PIXEL_PER_SQUARE);

        glVertex2f(x_a, y_a + PIXEL_PER_SQUARE);
        glVertex2f(x_a, y_a);
        glEnd();
    }
}

void fruits_draw()
{
    for(int i = 0; i < FRUIT_NUMBER; ++i)
    {
        glColor3f(0.0, 1.0, 0.0);
        glRectf(fruits[i].x * PIXEL_PER_SQUARE,
                fruits[i].y * PIXEL_PER_SQUARE,
                (fruits[i].x + 1) * PIXEL_PER_SQUARE,
                (fruits[i].y + 1) * PIXEL_PER_SQUARE);
    }
}

void snake_info_draw()
{
    char snakeLenth[100];
    char snakeScore[100];
    sprintf(snakeLenth, "LENTH:%d", snake.len);
    sprintf(snakeScore, "SCORE:%d", snake.score);
    drawWord(snakeLenth, 10, 510, 20, 255, 255, 0);
    drawWord(snakeScore, 200, 510, 20, 255, 255, 0);
    if(isGameOver)
    {
        drawWord(LOST_MSG, 450, 510, 20, 255, 0, 0);
    } else if(isPaused)
    {
        drawWord(PAUSE_MSG, 450, 510, 20, 255, 255, 0);
    }
}

void drawWord(char *word, float x_, float y_, float space, int red, int green,
              int blue)
{
    if(red < 0) red = 0;
    if(red > 255) red = 255;
    if(green < 0) green = 0;
    if(green > 255) green = 255;
    if(blue < 0) blue = 0;
    if(blue > 255) blue = 255;

    glColor3f(red / 255.f, green / 255.f, blue / 255.f);
    for(unsigned int i = 0; i < strlen(word); ++i)
    {
        glRasterPos2f(x_ + i * space, y_);
        char ch = word[i];
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)ch);
    }
}
