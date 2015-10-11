#include "ofApp.h"

void ofApp::setup() {
  screenWidth = ofGetWindowWidth();
  screenHeight = ofGetWindowHeight();

  frameWidth = 0;
  frameHeight = 0;
  frameCount = 0;

  frameRate = 10;
  prevTime = ofGetElapsedTimeMillis();
  currFrame = 0;
  isPlaying = true;

  inputPixels = NULL;
  outputPixels = NULL;

  loadFrames("bmosmall");
}

void ofApp::update() {
  if (isPlaying) {
    unsigned long long now = ofGetElapsedTimeMillis();
    unsigned long delta = now - prevTime;
    if (delta >= frameRate) {
      currFrame += delta / frameRate;
      if (currFrame >= frameCount) {
        currFrame = 0;
      }
      prevTime = now;

      currFrameChanged();
    }
  }
}

void ofApp::draw() {
  ofBackground(0);

  if (inputPixels != NULL) {
    ofSetColor(255);
    inputDrawImage.draw(
        (screenWidth - frameWidth) / 2,
        (screenHeight - frameHeight - frameHeight) / 2);
  }

  if (outputPixels != NULL) {
    ofSetColor(255);
    outputDrawImage.draw(
        (screenWidth - frameWidth) / 2,
        (screenHeight - frameHeight - frameHeight) / 2 + frameHeight);
  }
}

void ofApp::keyPressed(int key) {
}

void ofApp::keyReleased(int key) {
  switch (key) {
    case ' ':
      step();
      break;
    case '1':
      for (int i = 0; i < 5; i++) {
        cout << i << endl;
        step();
      }
      break;
    case '2':
      for (int i = 0; i < 10; i++) {
        cout << i << endl;
        step();
      }
      break;
    case '3':
      for (int i = 0; i < 50; i++) {
        cout << i << endl;
        step();
      }
      break;
    case '4':
      for (int i = 0; i < 100; i++) {
        cout << i << endl;
        step();
      }
      break;
    case 'f':
      isPlaying = !isPlaying;
      break;
    case 'h':
      currFrame--;
      if (currFrame < 0) {
        currFrame = frameCount - 1;
      }
      currFrameChanged();
      break;
    case 'l':
      currFrame++;
      if (currFrame >= frameCount) {
        currFrame = 0;
      }
      currFrameChanged();
      break;
    case 's':
      saveFrames(getOutputName());
      break;
  }
}

void ofApp::mouseMoved(int x, int y ) {
}

void ofApp::mouseDragged(int x, int y, int button) {
}

void ofApp::mousePressed(int x, int y, int button) {
}

void ofApp::mouseReleased(int x, int y, int button) {
}

void ofApp::windowResized(int w, int h) {
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
}

void ofApp::clearFrames() {
  if (inputPixels != NULL) delete[] inputPixels;
  if (outputPixels != NULL) delete[] outputPixels;

  inputPixels = NULL;
  outputPixels = NULL;

  frameCount = 0;
  frameWidth = 0;
  frameHeight = 0;
}

int ofApp::countFrames(string path) {
  int n = 0;
  ofFile file;
  while (file.doesFileExist(path + "/frame" + ofToString(n + 1, 0, 4, '0') + ".png")) n++;
  return n;
}

void ofApp::loadFrames(string path) {
  ofImage image;

  clearFrames();

  image.loadImage(path + "/frame0001.png");
  frameWidth = image.width;
  frameHeight = image.height;
  frameCount = countFrames(path);

  cout << "Loading " << frameCount << " frames " << endl
    << "\tPath: " << path << endl
    << "\tDimensions: " << frameWidth << "x" << frameHeight << endl
    << "\tSize: " << floor(frameCount * frameWidth * frameHeight * 3 / 1024 / 1024) << " MB" << endl;

  inputPixels = new unsigned char[frameCount * frameWidth * frameHeight * 3];
  outputPixels = new unsigned char[frameCount * frameWidth * frameHeight * 3];

  for (int frameIndex = 0; frameIndex < frameCount; frameIndex++) {
    image.loadImage(path + "/frame" + ofToString(frameIndex + 1, 0, 4, '0') + ".png");
    image.setImageType(OF_IMAGE_COLOR);

    for (int i = 0; i < frameWidth * frameHeight * 3; i++) {
      unsigned char c = image.getPixels()[i];
      inputPixels[frameIndex * frameWidth * frameHeight * 3 + i] = c;
      outputPixels[frameIndex * frameWidth * frameHeight * 3 + i] = c;
    }
  }

  inputDrawPixels = new unsigned char[frameWidth * frameHeight * 3];
  outputDrawPixels = new unsigned char[frameWidth * frameHeight * 3];

  currFrameChanged();

  cout << "Loading complete." << endl;
}

void ofApp::saveFrames(string path) {
  ofImage image;
  unsigned char* pixels = new unsigned char[frameWidth * frameHeight * 3];

  ofDirectory dir(path);
  if(!dir.exists()){
    dir.create(true);
  }

  for (int i = 0; i < frameCount; i++) {
    memcpy(pixels, outputPixels + (i * frameWidth * frameHeight * 3), frameWidth * frameHeight * 3);
    image.setFromPixels(pixels, frameWidth, frameHeight, OF_IMAGE_COLOR);
    image.saveImage(path + "/frame" + ofToString(i + 1, 0, 4, '0') + ".png");
  }

  delete[] pixels;
}

string ofApp::getOutputName() {
  int i = 1;
  while (true) {
    string path = "out" + ofToString(i, 0, 4, '0');
    ofDirectory dir(path);
    if (!dir.exists()) {
      return path;
    }
    i++;
  }
}

void ofApp::step() {
  for (int i = 0; i < frameCount - 1; i++) {
    for (int x = 0; x < frameWidth; x++) {
      for (int y = 0; y < frameHeight; y++) {
        sortPixel(x, y, i, i + 1);
      }
    }
  }
}

void ofApp::sortPixel(int x, int y, int frame0, int frame1) {
  ofColor c0 = getColor(x, y, frame0);
  ofColor c1 = getColor(x, y, frame1);
  float v0 = c0.getLightness();
  float v1 = c1.getLightness();

  if (v0 > v1) {
    setColor(x, y, frame0, c1);
    setColor(x, y, frame1, c0);
  }
}

ofColor ofApp::getColor(int x, int y, int frame) {
  ofColor result(
      outputPixels[frame * frameWidth * frameHeight * 3 + y * frameWidth * 3 + x * 3 + 0],
      outputPixels[frame * frameWidth * frameHeight * 3 + y * frameWidth * 3 + x * 3 + 1],
      outputPixels[frame * frameWidth * frameHeight * 3 + y * frameWidth * 3 + x * 3 + 2]);
  return result;
}

void ofApp::setColor(int x, int y, int frame, ofColor c) {
  outputPixels[frame * frameWidth * frameHeight * 3 + y * frameWidth * 3 + x * 3 + 0] = c.r;
  outputPixels[frame * frameWidth * frameHeight * 3 + y * frameWidth * 3 + x * 3 + 1] = c.g;
  outputPixels[frame * frameWidth * frameHeight * 3 + y * frameWidth * 3 + x * 3 + 2] = c.b;
}

void ofApp::currFrameChanged() {
  memcpy(inputDrawPixels, inputPixels + (currFrame * frameWidth * frameHeight * 3), frameWidth * frameHeight * 3);
  inputDrawImage.setFromPixels(inputDrawPixels, frameWidth, frameHeight, OF_IMAGE_COLOR);

  memcpy(outputDrawPixels, outputPixels + (currFrame * frameWidth * frameHeight * 3), frameWidth * frameHeight * 3);
  outputDrawImage.setFromPixels(outputDrawPixels, frameWidth, frameHeight, OF_IMAGE_COLOR);
}