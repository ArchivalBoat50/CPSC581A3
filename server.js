const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);


const portName = 'COM10';

const arduinoPort = new SerialPort({ path: portName, baudRate: 9600 });
const parser = arduinoPort.pipe(new ReadlineParser({ delimiter: '\n' }));

arduinoPort.on('open', () => {
  console.log('Serial Port Opened');
});

arduinoPort.on('error', (err) => {
  console.error('Serial Port Error: ', err.message);
});

parser.on('data', (data) => {
  console.log('Arduino:', data);
});

// Serve static files (HTML, CSS, JS)
app.use(express.static(__dirname));

// Handle socket connection
io.on('connection', (socket) => {
  console.log('Client connected');

  socket.on('dog_detected', () => {
    console.log('Dog detected - sending command to Arduino');
    arduinoPort.write('OPEN_DOOR\n');
  });

  socket.on('disconnect', () => {
    console.log('Client disconnected');
  });
});

// Start the server
const PORT = 3000;
server.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
