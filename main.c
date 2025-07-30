#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

extern void compute_acceleration(int, float*, int*);

int main() {
    int rows;

    printf("Enter number of cars: ");
    scanf_s("%dd", &rows);

    if (rows <= 0) {
        printf("Invalid count\n");
        return 1;
    }

    float* input_data = malloc(sizeof(float) * rows * 3);
    int* output_data = malloc(sizeof(int) * rows);

    if (!input_data || !output_data) {
        printf("Failed to allocate memory\n");
        return 1;
    }

    printf("Enter car data matrix:\n");
    for (int i = 0; i < rows; i++)
        scanf_s("%f, %f, %f", &input_data[i * 3], &input_data[i * 3 + 1], &input_data[i * 3 + 2]);

    compute_acceleration(rows, input_data, output_data);

    printf("\nAcceleration values (m/s2):\n");
    for (int i = 0; i < rows; i++)
        printf("%d\n", output_data[i]);

    free(input_data);
    free(output_data);

    return 0;
}
