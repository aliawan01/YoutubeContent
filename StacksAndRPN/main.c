#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STACK_SIZE 10

typedef struct {
    int stack[MAX_STACK_SIZE];
    int size;
} Stack;

bool push(Stack* stack, int value) {
    if (stack->size < MAX_STACK_SIZE) {
        stack->size++;
        stack->stack[stack->size] = value;
        return true;
    }

    return false;
}

int pop(Stack* stack) {
    if (stack->size > -1) {
        int value = stack->stack[stack->size];
        stack->size--;
        return value;
    }

    return 0;
}

int peek(Stack stack) {
    if (stack.size > -1) {
        return stack.stack[stack.size];
    }

    return 0;
}

int CalculateRPN(Stack stack) {
    Stack values = {0, -1};

    while (true) {
        int num = pop(&stack);
        char character = (char)num;

        if (character == '+' || character == '-' || character == '/' || character == '*') {
            int firstNum = pop(&values);
            int secondNum = pop(&values);

            switch (character) {
                case '+': push(&stack, firstNum+secondNum); break;
                case '-': push(&stack, firstNum-secondNum); break;
                case '*': push(&stack, firstNum*secondNum); break;
                case '/': push(&stack, firstNum/secondNum); break;
            }
        } 
        else {
            if (stack.size == 0) {
                return num;
            }

            push(&values, num);
        }

    }
}

int main() {
    Stack myStack = {0, -1};
    push(&myStack, '-');
    push(&myStack, 6);
    push(&myStack, '+');
    push(&myStack, '*');
    push(&myStack, 2);
    push(&myStack, 3);
    push(&myStack, 5);

    printf("RPN Result: %d", CalculateRPN(myStack));

    return 0;
}
