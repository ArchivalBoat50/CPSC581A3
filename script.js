// Initialize Socket.io
const socket = io();

let net;
const webcamElement = document.getElementById('webcam');
const statusElement = document.getElementById('status');

let dogDetected = false;
let lastDogDetectionTime = 0;
const detectionCooldown = 3000; // 3 seconds cooldown in milliseconds

async function app() {
  console.log('Loading MobileNet...');

  // Load the model.
  net = await mobilenet.load();
  console.log('Successfully loaded model');

  statusElement.innerText = 'Model loaded. Starting webcam...';

  // Setup webcam
  const webcam = await tf.data.webcam(webcamElement);

  statusElement.innerText = 'Webcam started. Classifying...';

  while (true) {
    const img = await webcam.capture();
    const result = await net.classify(img);

    const predictions = result.map(item => item.className.toLowerCase());

    const currentTime = Date.now();

    if (predictions.some(className => className.includes('dog'))) {
      if (!dogDetected && (currentTime - lastDogDetectionTime >= detectionCooldown)) {
        dogDetected = true;
        lastDogDetectionTime = currentTime;

        statusElement.innerText = 'Dog detected';
        statusElement.classList.remove('neutral', 'reject');
        statusElement.classList.add('approve');

        // Emit event to server when a dog is detected
        socket.emit('dog_detected');
      }
    } else if (predictions.some(className => className.includes('cat'))) {
      dogDetected = false; // Reset dog detection
      statusElement.innerText = 'Cat detected - REJECT';
      statusElement.classList.remove('neutral', 'approve');
      statusElement.classList.add('reject');
      // Do not emit any event to the server
    } else {
      dogDetected = false; // Reset dog detection
      statusElement.innerText = 'No dog or cat detected';
      statusElement.classList.remove('approve', 'reject');
      statusElement.classList.add('neutral');
    }

    // Dispose of the tensor to release memory
    img.dispose();

    // Wait for the next animation frame
    await tf.nextFrame();
  }
}

app();
