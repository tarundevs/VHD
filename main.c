#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mxc.h"
#include "audio.h"
#include "pcg.h"
//#include "file_arrays.h"
// Enum for heart disease classes
typedef enum {
    AS = 0,
    MR = 1,
    MS = 2,
    MVP = 3,
    N = 4,
    NUM_CLASSES = 5
} HeartClass;

// Structure for confusion matrix (5x5 for multi-class)
typedef struct {
    int matrix[5][5];  // [actual][predicted]
    int total_samples;
} ConfusionMatrix;

// Convert string class name to HeartClass enum
HeartClass str_to_class(const char* class_str) {
    if (strcmp(class_str, "AS") == 0) return AS;
    if (strcmp(class_str, "MR") == 0) return MR;
    if (strcmp(class_str, "MS") == 0) return MS;
    if (strcmp(class_str, "MVP") == 0) return MVP;
    if (strcmp(class_str, "N") == 0) return N;
    return NUM_CLASSES; // Error case
}

// Function to convert prediction to binary outcome
inline int check_prediction(HeartClass actual, HeartClass predicted) {
    return (actual == predicted) ? 1 : 0;
}

// Update confusion matrix with new prediction
inline void update_confusion_matrix(ConfusionMatrix* cm, HeartClass actual, HeartClass predicted)
{
    if (actual < NUM_CLASSES && predicted < NUM_CLASSES)
    {
        cm->matrix[actual][predicted]++;
        cm->total_samples++;
    }
}

// Initialize confusion matrix
void init_confusion_matrix(ConfusionMatrix* cm) {
    memset(cm->matrix, 0, sizeof(cm->matrix));
    cm->total_samples = 0;
}

// Calculate per-class metrics
void calculate_class_metrics(const ConfusionMatrix* cm, HeartClass class,
                           float* precision, float* recall, float* f1) {
    int true_positive = cm->matrix[class][class];
    int false_positive = 0;
    int false_negative = 0;

    // Calculate false positives and negatives
    for (int i = 0; i < NUM_CLASSES; i++) {
        if (i != class) {
            false_positive += cm->matrix[i][class];
            false_negative += cm->matrix[class][i];
        }
    }

    // Calculate metrics
    *precision = (true_positive + false_positive == 0) ? 0.0f :
                (float)true_positive / (true_positive + false_positive);

    *recall = (true_positive + false_negative == 0) ? 0.0f :
              (float)true_positive / (true_positive + false_negative);

    *f1 = (*precision + *recall == 0) ? 0.0f :
          2.0f * (*precision * *recall) / (*precision + *recall);
}

// Calculate overall accuracy
float calculate_accuracy(const ConfusionMatrix* cm) {
    if (cm->total_samples == 0) return 0.0f;

    int correct = 0;
    for (int i = 0; i < NUM_CLASSES; i++) {
        correct += cm->matrix[i][i];
    }
    return (float)correct / cm->total_samples;
}

// Print confusion matrix with class labels
void print_confusion_matrix(const ConfusionMatrix* cm) {
    static const char* class_names[] = {"AS", "MR", "MS", "MVP", "N"};

    printf("\nConfusion Matrix:\n");
    printf("------------------\n");
    printf("Actual  Predicted\n");
    printf("        AS   MR   MS   MVP  N\n");

    for (int i = 0; i < NUM_CLASSES; i++) {
        printf("%-7s", class_names[i]);
        for (int j = 0; j < NUM_CLASSES; j++) {
            printf(" %-4d", cm->matrix[i][j]);
        }
        printf("\n");
    }
}

// Print metrics for all classes
void print_metrics(const ConfusionMatrix* cm) {
    static const char* class_names[] = {"AS", "MR", "MS", "MVP", "N"};
    float precision, recall, f1;
    float overall_accuracy = calculate_accuracy(cm);

    printf("\nMetrics per class:\n");
    printf("------------------\n");
    for (int i = 0; i < NUM_CLASSES; i++) {
        calculate_class_metrics(cm, i, &precision, &recall, &f1);
        printf("%s:\n", class_names[i]);
        printf("  Precision: %.4f\n", precision);
        printf("  Recall:    %.4f\n", recall);
        printf("  F1 Score:  %.4f\n\n", f1);
    }
    printf("Overall Accuracy: %.4f\n", overall_accuracy);
}

int main(void) {
	printf("Initializingggg");
    // Initialize confusion matrix
    ConfusionMatrix cm;
    init_confusion_matrix(&cm);

    // Process 100 samples
    const int NUM_SAMPLES = 1;
    int valid_samples = 0;
    int err=0;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        printf("Processing sample %d\n", i);
//        err=audio_main();
//    }
//}


        // Get prediction from CNN
        char* predicted_str = pcg_main(i);

        // Determine actual class based on index
        const char* actual_str;
        switch (i % 5) {
            case 0: actual_str = "MS"; break;
            case 1: actual_str = "MR"; break;
            case 2: actual_str = "N"; break;
            case 3: actual_str = "AS"; break;
            case 4: actual_str = "MVP"; break;
            default: actual_str = "N"; break;
        }

        // Convert strings to HeartClass enum
        HeartClass actual = str_to_class(actual_str);
        HeartClass predicted = str_to_class(predicted_str);

        // Validate classes before updating matrix
//        if (actual != NUM_CLASSES && predicted != NUM_CLASSES) {
//            update_confusion_matrix(&cm, actual, predicted);

//            int is_correct = check_prediction(actual, predicted);
//            printf("Sample %d: %s (prediction %s = %d)\n",
//                   valid_samples++,
//                   (is_correct ? "correct" : "incorrect"),
//                   (is_correct ? "matched" : "didn't match"),
//                   is_correct);
//        } else {
//            printf("Warning: Invalid class detected for sample %d\n", i);
//        }
    }

    // Print results
    if (valid_samples > 0) {
        print_confusion_matrix(&cm);
        print_metrics(&cm);
//        LED_On(LED1); // Indicate completion
//    } else {
//        printf("Error: No valid samples processed\n");
//        LED_Toggle(LED1); // Indicate error
//    }
    }

    while (1) {
        __WFI(); // Wait for interrupt (low power mode)
    }
}

//int main(void)
//{
//	printf("Initializing LCD");
//	lcd();
//	printf("Initialized lcd");
//	LCD_String("12345");
//	return 0;
//}
