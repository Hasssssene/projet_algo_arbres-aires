#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "raymath.h"
#include <math.h>

#define MAX_CHILDREN 5

typedef struct NaryTreeNode {
    int data;
    int childCount;
    struct NaryTreeNode* children[MAX_CHILDREN];
} NaryTreeNode;

NaryTreeNode* root = NULL;
int inputValue = 0;
int parentValue = 0;
int levelValue = 1;
int searchResult = -1;

int searchState = 0;
Color searchRectangleColor = GRAY;

NaryTreeNode* currentNode = NULL;

int NodeExists(NaryTreeNode* node, int value) {
    if (node == NULL) {
        return 0;
    }

    if (node->data == value) {
        return 1;
    }

    for (int i = 0; i < node->childCount; ++i) {
        if (NodeExists(node->children[i], value)) {
            return 1;
        }
    }

    return 0;
}

void InsertNode(NaryTreeNode** node, int value, int parentValue, int level) {
    if (*node == NULL) {
        *node = (NaryTreeNode*)malloc(sizeof(NaryTreeNode));
        (*node)->data = value;
        (*node)->childCount = 0;
        for (int i = 0; i < MAX_CHILDREN; ++i) {
            (*node)->children[i] = NULL;
        }
    } else if ((*node)->data == parentValue && level > 1) {
        InsertNode(&((*node)->children[(*node)->childCount]), value, parentValue, level - 1);
        (*node)->childCount++;
    } else {
        int i;
        for (i = 0; i < (*node)->childCount; ++i) {
            if ((*node)->children[i]->data == parentValue) {
                InsertNode(&((*node)->children[i]), value, parentValue, level);
                break;
            }
        }

        if (i == (*node)->childCount) {
            for (i = 0; i < (*node)->childCount; ++i) {
                InsertNode(&((*node)->children[i]), value, parentValue, level);
            }
        }
    }
}

void DeleteTree(NaryTreeNode* node) {
    if (node != NULL) {
        for (int i = 0; i < node->childCount; ++i) {
            DeleteTree(node->children[i]);
        }
        free(node);
    }
}

void DeleteNode(NaryTreeNode** node, int value) {
    if (*node != NULL) {
        for (int i = 0; i < (*node)->childCount; ++i) {
            if ((*node)->children[i]->data == value) {
                DeleteTree((*node)->children[i]);
                for (int j = i; j < (*node)->childCount - 1; ++j) {
                    (*node)->children[j] = (*node)->children[j + 1];
                }
                (*node)->childCount--;
                return;
            } else {
                DeleteNode(&((*node)->children[i]), value);
            }
        }
    }
}

void SearchNode(NaryTreeNode** node, int value, int x, int y, int radius, float parentAngle) {
    if ((*node) != NULL) {
        currentNode = *node;
        if ((*node)->data == value) {
            searchResult = 1;
            DrawCircle(x, y, radius, GREEN);
            searchState = 2;
            return;
        }

        searchResult = 0;

        for (int i = 0; i < (*node)->childCount; i++) {
            float angle = parentAngle + (180.0 / ((*node)->childCount)) * (i + 0.5);
            float length = radius * 2;
            Vector2 position = { cos(DEG2RAD * angle), sin(DEG2RAD * angle) };
            position = Vector2Scale(position, length);
            position = Vector2Add(position, (Vector2){ (float)x, (float)y });

            SearchNode(&((*node)->children[i]), value, position.x, position.y, radius / 2, angle);

            if (searchResult == 1) {
                DrawCircle(x, y, radius, GREEN);
                searchState = 2;
                return;
            }
        }

        if (searchResult == 0) {
            searchState = 3;
            DrawCircle(x, y, radius, YELLOW);
        }
    }
}

void DrawTreeNode(NaryTreeNode *node, int x, int y, int radius, float parentAngle, int level) {
    if (node == NULL) {
        return;
    }

    const char *text = TextFormat(" %d", node->data);
    int fontSize = (int)(radius * 0.45);

    DrawCircle(x, y, radius, LIGHTGRAY);
    DrawText(text, x - MeasureText(text, fontSize) / 2, y - fontSize / 2, fontSize, BLACK);

    for (int i = 0; i < node->childCount; i++) {
        if (node->children[i] == NULL) {
            printf("Child %d of node %d is NULL\n", i, node->data);
            break;
        }

        float angle = parentAngle + (180.0 / (node->childCount)) * (i + 0.5);
        float baseLength = radius * 3;
        float length = baseLength - (level - 1) * 10;

        Vector2 position = { cos(DEG2RAD * angle), sin(DEG2RAD * angle) };
        position = Vector2Scale(position, length);
        position = Vector2Add(position, (Vector2){ (float)x, (float)y });
        DrawLine(x, y + fontSize / 2, position.x, position.y, LIGHTGRAY);

        DrawTreeNode(node->children[i], position.x, position.y, radius / 2.1, angle, level + 1);
    }
}

int main() {
    const int screenWidth = 1800;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "N-ary Tree");

    Rectangle valueBox = {10, screenHeight - 80, 120, 30};
    Rectangle parentBox = {140, screenHeight - 80, 120, 30};
    Rectangle levelBox = {270, screenHeight - 80, 120, 30};
    Rectangle insertButton = {10, screenHeight - 40, 120, 30};
    Rectangle deleteButton = {140, screenHeight - 40, 120, 30};
    Rectangle resetButton = {270, screenHeight - 40, 120, 30};
    Rectangle searchButton = {530, screenHeight - 80, 120, 30};

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();

        if ((CheckCollisionPointRec(mousePos, insertButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (IsKeyPressed(32))) {
            InsertNode(&root, inputValue, parentValue, levelValue);
            inputValue = 0;
            levelValue++;
        }

        if (CheckCollisionPointRec(mousePos, resetButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            DeleteTree(root);
            root = NULL;
        }

        if (CheckCollisionPointRec(mousePos, deleteButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            DeleteNode(&root, inputValue);
            inputValue = 0;
        }

        if ((CheckCollisionPointRec(mousePos, searchButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (IsKeyPressed(KEY_ENTER))) {
            searchResult = -1;
            currentNode = NULL;
            searchState = 1;
            SearchNode(&root, inputValue, screenWidth / 2, screenHeight / 3, 70, 0);
        }

        int key = GetKeyPressed();
        if ((key >= KEY_ZERO) && (key <= KEY_NINE)) {
            if (CheckCollisionPointRec(mousePos, valueBox)) {
                inputValue = inputValue * 10 + (key - KEY_ZERO);
            } else if (CheckCollisionPointRec(mousePos, parentBox)) {
                parentValue = parentValue * 10 + (key - KEY_ZERO);
            } else if (CheckCollisionPointRec(mousePos, levelBox)) {
                levelValue = levelValue * 10 + (key - KEY_ZERO);
            }
        } else if ((key == KEY_BACKSPACE)) {
            if (CheckCollisionPointRec(mousePos, valueBox)) {
                if (inputValue <= 10) {
                    inputValue = 0;
                }
                inputValue /= 10;
            } else if (CheckCollisionPointRec(mousePos, parentBox)) {
                parentValue /= 10;
            } else if (CheckCollisionPointRec(mousePos, levelBox)) {
                levelValue /= 10;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleRec(valueBox, GRAY);
        DrawText("Value :", valueBox.x + 10, valueBox.y + 10, 16, BLACK);
        DrawText(TextFormat("%d", inputValue), valueBox.x + 80, valueBox.y +7, 20, BLACK);

        DrawRectangleRec(parentBox, GRAY);
        DrawText("Parent :", parentBox.x + 10, parentBox.y + 10, 16, BLACK);
        DrawText(TextFormat("%d", parentValue), parentBox.x + 80, parentBox.y+7 , 20, BLACK);

        DrawRectangleRec(levelBox, GRAY);
        DrawText("Level :", levelBox.x + 10, levelBox.y + 10, 16, BLACK);
        DrawText(TextFormat("%d", levelValue), levelBox.x + 80, levelBox.y+7 , 20, BLACK);

        DrawRectangleRec(insertButton, GRAY);
        DrawText("Insert Node", insertButton.x + 10, insertButton.y + 10, 15, BLACK);
        DrawRectangleRec(deleteButton, GRAY);
        DrawText("Delete Tree", deleteButton.x + 10, deleteButton.y + 10, 15, BLACK);
        DrawRectangleRec(resetButton, GRAY);
        DrawText("Reset Tree", resetButton.x + 10, resetButton.y + 10, 15, BLACK);

        DrawRectangleRec(searchButton, searchRectangleColor);
        DrawText("Search", searchButton.x + 10, searchButton.y + 10, 15, BLACK);

        DrawTreeNode(root, screenWidth / 2, screenHeight / 3, 70, 0, 1);

        switch (searchState) {
            case 0: searchRectangleColor = GRAY; break;
            case 1: searchRectangleColor = YELLOW; break;
            case 2: searchRectangleColor = GREEN; break;
            case 3: searchRectangleColor = RED; break;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}





























































































































































































































































































































































 

















































 









  








 

   




















 



























         }
        
     












 


    





 
 

















 





        
        


     

      



        
        












