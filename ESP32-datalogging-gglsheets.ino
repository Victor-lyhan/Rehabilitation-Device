#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>

#include <GS_SDHelper.h>

#define WIFI_SSID "Victor Han" // "SpectrumSetup-D1 // Victor Han"
#define WIFI_PASSWORD "victorhan2007" // "formalocean637 //victorhan2007"

#define PROJECT_ID "esp32-datalogging-2024"

#define CLIENT_EMAIL "esp32-datalogging@esp32-datalogging-2024.iam.gserviceaccount.com"

const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCZImwUPWp635Hc\nPHKCRO1Yxzu9Z6HG1Vesgk15FU+V+YGzG9JsNZL9JWzJ7c4icJ1CprqhNDCQACT9\nycxxWDFSfixdCI8r9HgRojrGIm03vohkCeHmI9oGHw0aIkfLkIyI/NW4g4pwyzPB\nrVhyQxbr16GmcXjUOooanIcbqrTpjktYEHu/Us8gfyXhID/7n73jNlOu5pbhVNOd\n34S2L1p4/9Vcg+Ab5M8Y1VnyvCTIm0g5LG/6wKkCc8pTG8T07QBx33NoZzmrYJMu\nADGlxtodhwQEsK/MOUfGkcOm3bV+x4d17dI+dPcIeSIMlEcegZrtFgNs03FqggMC\nr7WOxBKJAgMBAAECggEAEULMh5wunZVdcG44gro4k1pjFPk9RoIeD6S2YoYbl5jt\nXN4vd0wIpVPWceJwwE1iTzTSR5ZwPOMPSF4P7Ids3n1jYcSJDYLRFT0XPpB7MB6K\nwCqkVcJycmCF+Q1RJhRv4DG4+MM2/nohJjJ0YZjHWM56+vMXvrsEfcsT/Z3d0OTb\nkzsf1pzvK3jvtPwlMSruHBPsDx7dSrG+F15aOvT+jSPG3LTDU2y79X/kanrjSUCb\nb54rillKTkQpspC6J+gl0/ucYKFlJQdPlplFc3cZC2qaH6auXcnhRq5z5n1t6h9N\nQO0vDR5Zb0w28aNmGnUkAffX2KZPNVAWGEJOqiS0QwKBgQDVRns8X+ELBWCTxJxD\nM8uVcCCsnwYbMb2lEq+UuoxPNjnpdbr0w/8Uy+orfhN3SfCU3GTfpH1Cq5Actz4f\ngyGIUvIsO0EcxY/6BHlxQRs3nO6G/cjhHuQOsUJDA/ZAOiS2p/Y7wTVyO2R2VZxz\nW04+cMvV4yL3QBYE+PPz7aQogwKBgQC3z7QKp6HZ5rTPWVV6/9x0N0rmistBIQUb\nsAzdC4SuL2stmIk2GTvcOCR/WxFDGxWfBKG0L8SNbY378Bi9/51IoLdyYeU60aQ9\n9EXTAZRVvd8i6QRN75n5xFlpMEugAFdn0mHLMEWduuS2O1NY/1jRhDO0eNAaw0vt\nq0YeLSyzAwKBgQCm5uRUKBg9/rRIj4lWEfeU982qKTynFXL69rePJbnd12rUp3xq\nReOeC8IyzRRAuw1aAWuzlM8t2IML5d7o3D7mpzq7oJdhA76bgtri5Xd+OwJzg7Gr\ncvg4A0H+neAc/SN0CddphdbkkVQaQv8PS3nc5Xqu5HSdl3pj48tNrkBULwKBgQCf\nkJM+3J7IRlqX1Xk7OXnq+UTwNXSUqVGB46aHDUS04T0ta8lRST6JwcMNO8UUeCKg\n1Kh7fywligFDv/36kje7oKkfhpQ0J2Fo1vD2Z9yZQia+a4HtFh6YkDb99Yrw7trx\nqdRzSwhkEKYS/BUyoRg9aYJpHKerU/ZJ7/ECqTyweQKBgGv8egzq/dFcr0lAb0ar\n98Swbz4P8njfVEPvYrqkwhbUnbtlhqzyfR7Yow1Kwv9TuB2uIqxkCBLa9H2ehFq/\nKcUzbD1WNxTp3OZn96fHmlRVjtFkA7+rGSm/xizaLzfDXeR8tP8k/D9WbbG4yrhI\ngsmZ8kZY7w8BEFFeocv7oTiN\n-----END PRIVATE KEY-----\n";

const char spreadsheetId[] = "18i4esnH7K-9-m6CvFqQbgURT-YsJAaJOgORS_StoX54";

const char* ntpServer = "pool.ntp.org";

unsigned long epochTime;

int userID = 0;
int bendingParameters[2] = { 0, 0 };
int tremorRate = 0;

void Decode(String packet) {
  if (packet.length() > 0) {
    int count = 0;
    String nPacket = packet.substring(2);

    String parameter = "";
    for (int i = 0; i < nPacket.length(); i++) {
      if (nPacket.charAt(i) != '-') {
        parameter += nPacket.charAt(i);
      } else {
        if (count == 0) {
          userID = parameter.toInt();
          count++;
        } else if (packet.startsWith("1-")) {
          bendingParameters[count-1] = parameter.toInt();
          count++;
        } else if (packet.startsWith("2-")){
          tremorRate = parameter.toInt();
          count++;
        }
        parameter = "";
      }
    }
  }
}

void tokenStatusCallback(TokenInfo info);

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return (0);
  }
  time(&now);
  return now;
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.println();
  Serial.println();

  configTime(0, 0, ntpServer);

  GSheet.printf("ESP Google Sheet Client v%s\n\n", ESP_GOOGLE_SHEET_CLIENT_VERSION);

  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  GSheet.setTokenCallback(tokenStatusCallback);

  GSheet.setPrerefreshSeconds(60);

  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
}

void loop() {
  bool ready = GSheet.ready();

  String inString = "";
  while (Serial2.available() > 0) {
    inString += char(Serial2.read());
  }
  Decode(inString);

  if (ready && bendingParameters[0] != 0) {
    FirebaseJson response;

    Serial.println("\nAppend spreadsheet values...");
    Serial.println("----------------------------");

    FirebaseJson valueRange;

    epochTime = getTime();

    valueRange.add("majorDimension", "COLUMNS");
    valueRange.set("values/[0]/[0]", userID);
    valueRange.set("values/[1]/[0]", epochTime);
    valueRange.set("values/[2]/[0]", bendingParameters[0]);
    valueRange.set("values/[3]/[0]", bendingParameters[1]);

    bool success = GSheet.values.append(&response, spreadsheetId, "Sheet1!A1", &valueRange);
    if (success) {
      response.toString(Serial, true);
      valueRange.clear();
      bendingParameters[0] = 0;
      bendingParameters[1] = 0;
    } else {
      Serial.println(GSheet.errorReason());
    }
    Serial.println();
    Serial.println(ESP.getFreeHeap());
  }

  if (ready && tremorRate != 0) {
    FirebaseJson response;

    Serial.println("\nAppend spreadsheet values...");
    Serial.println("----------------------------");

    FirebaseJson valueRange;

    epochTime = getTime();

    valueRange.add("majorDimension", "COLUMNS");
    valueRange.set("values/[0]/[0]", userID);
    valueRange.set("values/[1]/[0]", epochTime);
    valueRange.set("values/[2]/[0]", tremorRate);

    bool success = GSheet.values.append(&response, spreadsheetId, "Sheet1!G2", &valueRange);
    if (success) {
      response.toString(Serial, true);
      valueRange.clear();
      tremorRate = 0;
    } else {
      Serial.println(GSheet.errorReason());
    }
    Serial.println();
    Serial.println(ESP.getFreeHeap());
  }
}

void tokenStatusCallback(TokenInfo info) {
  if (info.status == token_status_error) {
    GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
  } else {
    GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
  }
}
