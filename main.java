import processing.serial.*;

Serial myPort;

String rawData = "";
float distanceInPixels = 0;

int angle = 0;
int distance = 0;

boolean showRedLine = false;

float radarRadius;

void setup()
{
  size(1080, 720);
  smooth();

  radarRadius = (width - 50) / 2.0;

  myPort = new Serial(this, "/dev/cu.usbmodem11301", 9600);
  myPort.bufferUntil('.');
}

void draw()
{
  fill(0, 4);
  noStroke();
  rect(0, 0, width, height - 50);

  drawRadar();
  drawObject();
  drawLine();

  fill(0);
  rect(0, height - 50, width, 50);
}

void drawRadar()
{
  pushMatrix();
  translate(width / 2, height - 50);

  noFill();
  stroke(100, 250, 25);
  strokeWeight(1);

  int numberOfCircles = 4;

  for (int i = 1; i <= numberOfCircles; i++)
  {
    float r = radarRadius * i / numberOfCircles;
    arc(0, 0, r * 2, r * 2, PI, TWO_PI);
  }

  popMatrix();
}

void drawLine()
{
  pushMatrix();
  strokeWeight(10);
  stroke(50,250,50);
  float centerX = width/2;
  float centerY = height-height*0.075;
  float lineLength = height * 0.75;
  float x2 = centerX + lineLength * cos(radians(angle));
  float y2 = centerY + lineLength * sin(radians(angle));
  line(centerX,centerY,x2,y2);
  popMatrix();
}

void drawObject()
{
  if (!showRedLine) return;

  pushMatrix();
  translate(width / 2, height - 50);

  stroke(255, 0, 0);
  strokeWeight(2);

  float x = distanceInPixels * cos(radians(angle));
  float y = -distanceInPixels * sin(radians(angle));

  line(0, 0, x, y);

  popMatrix();
}

void serialEvent(Serial myPort)
{
  rawData = myPort.readStringUntil('.');

  if (rawData == null) return;

  rawData = trim(rawData);
  rawData = rawData.substring(0, rawData.length() - 1);

  int commaIndex = rawData.indexOf(',');

  if (commaIndex == -1) return;

  angle = int(rawData.substring(0, commaIndex));
  distance = int(rawData.substring(commaIndex + 1));

  if (distance > 0)
  {
    showRedLine = true;
    distanceInPixels = map(distance, 0, 40, 0, radarRadius);
  }
  else
  {
    showRedLine = false;
    distanceInPixels = 0;
  }
}
