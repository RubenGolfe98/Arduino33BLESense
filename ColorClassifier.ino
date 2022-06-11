/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <TensorFlowLite.h>


#include "sine_model_data.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include <Arduino_APDS9960.h>

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;

// Class mapping
const char* CLASSES[] = {
"Azul",
"Manzana",
"Naranja"
};
#define NUM_CLASSES (sizeof(CLASSES) / sizeof(CLASSES[0]))

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.
constexpr int kTensorArenaSize = 8 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

int prox;
float red, green, blue;

// The name of this function is important for Arduino compatibility.
void setup() {
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_sine_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
     Serial.println("Las versiones de TF no coinciden!");
     while(1);
   }
  
  Serial.begin(9600);
  while (!Serial);

   if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
  }


  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::ops::micro::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Keep track of how many inferences we have performed.
  inference_count = 0;
}

// The name of this function is important for Arduino compatibility.
void loop() {

  while (!APDS.colorAvailable()) {
    delay(5);
  }

 int r, g, b, a;
  // Reading the color.
  APDS.readColor(r, g, b, a);

  if (APDS.proximityAvailable()) {
    // Read the proximity where:
    // - 0   => close
    // - 255 => far
    // - -1  => error
    int proximity = APDS.readProximity();
    
  
    if (proximity==0 && a >100  ){
      //Normalize
      //Red
      red=(r+g+b) ;
      red=r/red;
      //Green
      green=(r+g+b) ;
      green=g/green;
      //Blue
      blue=(r+g+b);
      blue=b/blue;
    }
  
  // input sensor data to tensorflow
  input->data.f[0] = red;
  input->data.f[1] = green;
  input->data.f[2] = blue;

  // Run inference, and report any error
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    Serial.println("Invocación fallida!");
    while (1);
    return;
  }

  // OUTPUT
  for (int i = 0; i < NUM_CLASSES; i++) {
    Serial.print(CLASSES[i]);
    Serial.print(" ");
    Serial.print(int(output->data.f[i] * 100));
    Serial.print("%\n");
  }
  Serial.println();
  delay(3000);
  }
}
