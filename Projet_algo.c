#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "raymath.h"
#include <math.h>
#include <time.h>
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
int itexists=0;
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
    // Check if the value already exists in the tree
    if (NodeExists(*node, value)) {
        itexists=1;
        levelValue--;
         return;
        
    }
    itexists=0;

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
                break;  // Break after finding the correct child
            }
        }

        // If the loop didn't break, then the parentValue was not found in the current node's children
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

void SearchNode(NaryTreeNode** node, int value, int x, int y,int Width ,  int levelSpacing,int radius) {

    if ((*node) != NULL) {
        currentNode = *node;
        if ((*node)->data == value) {
            searchResult = 1;
            DrawCircle(x, y, radius+5, GREEN);
            searchState = 2;
            return;
        }else{
            
            DrawCircle(x, y, radius+5, YELLOW);
            searchResult = 0;
        }

      int horizontalSpacing=Width / (((*node)->childCount)+1);
        int totalWidth = horizontalSpacing * (((*node)->childCount)-1);
        int startX = x - totalWidth / 2;

        for (int i = 0; i < (*node)->childCount; ++i) {
            int childX = startX + i * horizontalSpacing;
            int childY = y + levelSpacing;
            DrawLine(x, y, childX, childY, LIGHTGRAY);    
        

            SearchNode(&((*node)->children[i]), value, childX, childY, Width /(((*node)->childCount)),  levelSpacing/1.5 , radius/1.5);
            if (searchResult == 1) {                           
                searchState = 2;               
                return;
            } 
        }

        if (searchResult == 0) {
            searchState = 3;

        }      
    }
}


void DrawTree(NaryTreeNode* node, int x, int y,int Width ,  int levelSpacing,int radius) {
   
    
    if (node != NULL) {
        DrawCircle(x, y, radius, LIGHTGRAY);
        DrawText(TextFormat("%d", node->data), x - 10, y - 10, 20, BLACK);
        int horizontalSpacing=Width / ((node->childCount)+1);
        int totalWidth = horizontalSpacing * ((node->childCount)-1);
        int startX = x - totalWidth / 2;

        for (int i = 0; i < node->childCount; ++i) {
            int childX = startX + i * horizontalSpacing;
            int childY = y + levelSpacing;
            DrawLine(x, y, childX, childY, LIGHTGRAY);
            DrawTree(node->children[i], childX, childY, Width /((node->childCount)),  levelSpacing/1.5 , radius/1.5);
        }
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

        if ((CheckCollisionPointRec(mousePos, insertButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (IsKeyPressed(KEY_ENTER))) {
            InsertNode(&root, inputValue, parentValue, levelValue);
            inputValue = 0;
            levelValue++;
        }

        if ((CheckCollisionPointRec(mousePos, resetButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) {
            DeleteTree(root);
            root = NULL;
            levelValue=1;
            inputValue = 0;
            parentValue = 0;
        }

        if ((CheckCollisionPointRec(mousePos, deleteButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (IsKeyPressed(KEY_DELETE))) {
            DeleteNode(&root, inputValue);
            inputValue = 0;
            levelValue=levelValue-1;
        }

        if ((CheckCollisionPointRec(mousePos, searchButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || (IsKeyDown(32))) {
            searchResult = -1;
            searchState = 0;
           SearchNode(&root, inputValue,screenWidth / 2, 150,1900, 200,70);
                 
          
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

        DrawTree(root, screenWidth / 2, 150,1900, 200,70);
        
        DrawText("Press enter to insert ", 10, 700, 20, GREEN);
         DrawText("Press delete to delete ", 10, 740, 20, RED);
          DrawText("hold space to search ", 10, 780, 20, BLUE);

        switch (searchState) {
            case 0: searchRectangleColor = GRAY; break;
            case 1: searchRectangleColor = YELLOW;break;
            case 2: searchRectangleColor = GREEN;
            DrawText("Value exists", 10, 80, 20, GREEN);
            break;
            case 3: searchRectangleColor = RED;
            DrawText("Value does not exist", 10, 80, 20, RED);
            break;
        }
         if(itexists==1){
             DrawText("Error: Value already exists", 10, 30, 20, RED);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
} 