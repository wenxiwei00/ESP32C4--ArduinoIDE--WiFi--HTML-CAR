// WebPage.h From GPT
#ifndef WebPage
#define WebPage

const char WEBPAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Motor Control</title>
<style>
  body {
    font-family: Arial, sans-serif;
    text-align: center;
    margin: 0;
    padding: 0;
    background-color: #f0f0f0;
  }
  .container {
    margin: 50px auto;
    padding: 20px;
    max-width: 400px;
    background-color: #fff;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
  }
  h2 {
    color: #333;
  }
  label {
    font-weight: bold;
    color: #555;
  }
  input[type=range] {
    width: 100%;
    margin: 15px 0;
  }
  span {
    display: block;
    margin-top: 10px;
    font-size: 1.2em;
    color: #333;
  }
  button {
    margin: 10px;
    padding: 10px 20px;
    font-size: 1em;
  }
  .slider-container {
    margin: 15px 0;
  }
  .speed-display {
    margin: 10px 0;
  }
  #stopButton {
    background-color: red;
    color: white;
  }
</style>
</head>
<body>
<div class="container">
  <h2>Motor Control Panel</h2>
  
  <!-- Left motor control -->
  <div>
    <label for="leftMotor">Left Speed (-16383 to 16383):</label>
    <input type="range" id="leftMotor" min="-16383" max="16383" value="0" oninput="updateSpeedDisplay('left')">
    <span id="leftSpeedVal">0</span>
  </div>

  <div>
    <button id="left">Left</button>
    <button id="forward">Forward</button>
    <button id="right">Right</button>
    <button id="backward">Back</button>
  </div>

  <!-- Emergency stop button -->
  <div>
    <button onclick="emergencyStop()">Stop</button>
  </div>
  
  <!-- Right motor control -->
  <div>
    <label for="rightMotor">Right Speed (-16383 to 16383):</label>
    <input type="range" id="rightMotor" min="-16383" max="16383" value="0" oninput="updateSpeedDisplay('right')">
    <span id="rightSpeedVal">0</span>
  </div>

  <div>
    <span>Left Speed: <span id="leftSpeed">0</span> pps</span>
    <span>Right Speed: <span id="rightSpeed">0</span> pps</span>
  </div>
</div>
<script>

// Event Listeners for New Buttons //set for the new botton
document.getElementById("forward").addEventListener("click", function() {
  sendSpeedValues(16383, 16383);
});

document.getElementById("left").addEventListener("click", function() {
  sendSpeedValues(-6000, 6000);
});

document.getElementById("right").addEventListener("click", function() {
  sendSpeedValues(6000, -6000);
});

document.getElementById("backward").addEventListener("click", function() {
  sendSpeedValues(-16383, -16383);
});

// Function to send speed values to the server
function sendSpeedValues(leftSpeed, rightSpeed) {
  document.getElementById("leftMotor").value = leftSpeed;
  document.getElementById("rightMotor").value = rightSpeed;
  updateSpeedDisplay('left');
  updateSpeedDisplay('right');
}

// Function to update speed display and send values to Arduino using GET requests
function updateSpeedDisplay(motor) {
  var leftSpeedValue = document.getElementById("leftMotor").value;
  var rightSpeedValue = document.getElementById("rightMotor").value;

  // Update the display value
  document.getElementById(motor + 'SpeedVal').innerText = motor === 'left' ? leftSpeedValue : rightSpeedValue;

  // Construct the GET request URL with the speed parameters for both motors
  var url = `/set?leftSpeed=${leftSpeedValue}&rightSpeed=${rightSpeedValue}`;

  // Send the GET request to the server
  fetch(url)
  .then(response => response.text())
  .then(data => console.log(data))
  .catch(error => console.error('Error:', error));
}

// Function for emergency stop
function emergencyStop() {
  fetch("/stop");
  document.getElementById("leftMotor").value = 0;
  document.getElementById("rightMotor").value = 0;
  document.getElementById("leftSpeedVal").innerText = 0;
  document.getElementById("rightSpeedVal").innerText = 0;
}

// Function to update the speed display with the encoder values from Arduino
function updateSpeed() {
  fetch("/speed")
  .then(response => response.json())
  .then(data => {
    // Assuming 'data' is an object with properties 'left' and 'right' for the speeds
    document.getElementById("leftSpeed").innerText = data.left + " pps";
    document.getElementById("rightSpeed").innerText = data.right + " pps";
  })
  .catch(error => console.error('Error fetching speed:', error));
}

// Call updateSpeed every second to refresh the encoder speed display
setInterval(updateSpeed, 1000);


</script>
</body>
</html>
)rawliteral";
#endif /* WebPage */
