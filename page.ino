void handleRoot() {
  // Build the HTML for the web page dynamically
  String html = "<!DOCTYPE html><html><head><title>Sumo bot</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; }";
  html += "h1 { color: #333; }";
  html += ".container { max-width: 400px; margin: 50px auto; padding: 20px; background-color: #fff; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
  html += ".status { font-size: 1.2em; margin: 20px 0; }";
  html += ".button { display: inline-block; padding: 15px 30px; font-size: 1.2em; cursor: pointer; text-align: center; text-decoration: none; outline: none; color: #fff; border: none; border-radius: 5px; box-shadow: 0 4px #999; }";
  html += ".button-on { background-color: #4CAF50; }";   // Green
  html += ".button-off { background-color: #f44336; }";  // Red
  html += ".button:active { background-color: #333; box-shadow: 0 2px #666; transform: translateY(2px); }";
  html += "</style></head>";
  html += "<body><div class='container'>";
  html += "<h1>ESP32 Driver Control</h1>";

  // Display the current state
  html += "<div class='status'>The driver is currently: <strong>";
  if (driverState) {
    html += "ON";
  } else {
    html += "OFF";
  }
  html += "</strong></div>";

  // Display the correct button based on the current state
  if (driverState) {
    // If it's ON, show a button to turn it OFF
    html += "<a href='/off'><button class='button button-off'>Turn OFF</button></a>";
  } else {
    // If it's OFF, show a button to turn it ON
    html += "<a href='/on'><button class='button button-on'>Turn ON</button></a>";
  }
  
  html += "</div></body></html>";
  
  server.send(200, "text/html", html); // Send the HTML page to the client
}

void handleOn() {
  driverState = true;
  Serial.println("Driver turned ON");
  // Redirect the client back to the root page
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", ""); 
}

// This function handles the "/off" URL
void handleOff() {
  driverState = false;
  Serial.println("Driver turned OFF");  
  // Redirect the client back to the root page
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

