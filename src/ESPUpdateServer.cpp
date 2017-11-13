#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "espconfig.h"
#include "ExternalData.h"

#define EXTERNALDATA_ON_SYNC_SERVER false

#if SerialDebug == true
#define DBG_PRINT(...) DebugPort.print(__VA_ARGS__)
#define DBG_PRINTLN(...) DebugPort.println(__VA_ARGS__)
#else
#define DBG_PRINT(...)
#define DBG_PRINTLN(...)
#endif

#if (DEVELOPMENT_OTA == true) || (DEVELOPMENT_FILEMANAGER == true)
static ESP8266WebServer server(UPDATE_SERVER_PORT);
#endif

#if DEVELOPMENT_OTA == true
static ESP8266HTTPUpdateServer httpUpdater;
#endif

#if DEVELOPMENT_FILEMANAGER == true

#include "data_edit_html_gz.h"

#define SPIFFS_FORMAT_PATH     "/format-spiffs"
#define SPIFFS_FORMATTING_PATH "/exeformat-spiffs"


static const char PROGMEM spiffsformat_html[]  =   { 0x1f, 0x8b, 0x08, 0x08, 0x17, 0xcf, 0x09, 0x5a, 0x02, 0x03, 0x66, 0x6f,
  0x72, 0x6d, 0x61, 0x74, 0x66, 0x73, 0x2e, 0x68, 0x74, 0x6d, 0x6c, 0x00,
  0x55, 0x53, 0xcb, 0x6e, 0xdb, 0x30, 0x10, 0xfc, 0x95, 0x2d, 0x2f, 0x22,
  0x11, 0x45, 0xb6, 0xfb, 0x48, 0x1b, 0xe8, 0x71, 0x08, 0x90, 0x00, 0xb9,
  0x05, 0x4d, 0x81, 0x1e, 0x12, 0x17, 0xa0, 0xa8, 0x95, 0x45, 0x88, 0xa6,
  0x08, 0x89, 0xaa, 0x63, 0x98, 0xfa, 0xf7, 0x92, 0x92, 0xe3, 0xb6, 0x80,
  0x00, 0x72, 0x77, 0xb9, 0x33, 0xc3, 0xe1, 0x2a, 0x6b, 0xec, 0x5e, 0x15,
  0x59, 0x83, 0xbc, 0x2a, 0x32, 0x2b, 0xad, 0xc2, 0xe2, 0xf9, 0xe9, 0xf1,
  0xe1, 0xe1, 0x19, 0x1e, 0xba, 0x7e, 0xcf, 0x6d, 0xb6, 0x5a, 0x92, 0xd9,
  0x20, 0x7a, 0x69, 0x6c, 0x81, 0xbf, 0xb9, 0xa2, 0xf5, 0xa8, 0x85, 0x95,
  0x9d, 0xa6, 0x26, 0xe6, 0xb1, 0x88, 0xdb, 0x18, 0xe3, 0x9e, 0x9d, 0x30,
  0xbf, 0xe4, 0x05, 0x3b, 0xf5, 0x68, 0xc7, 0x5e, 0x83, 0x48, 0x6c, 0xf7,
  0x6c, 0x7b, 0xa9, 0x77, 0x94, 0xb3, 0x29, 0x95, 0x35, 0xfd, 0x10, 0x45,
  0x49, 0x8f, 0x46, 0x71, 0x81, 0x74, 0xf5, 0x6b, 0x15, 0x2f, 0x55, 0xc6,
  0x4e, 0x87, 0x46, 0x2a, 0xa4, 0xe2, 0xfa, 0x9a, 0xf5, 0x2f, 0x7e, 0x65,
  0xdb, 0xbc, 0x7d, 0x11, 0x5b, 0xe7, 0xc2, 0x3e, 0x6d, 0xf3, 0x97, 0x0b,
  0x3a, 0x5e, 0xd0, 0xfd, 0xc1, 0xed, 0xb4, 0x4d, 0xff, 0x61, 0x7e, 0x2f,
  0x45, 0xaf, 0xaf, 0x87, 0xab, 0x68, 0x4a, 0x45, 0xbe, 0x99, 0xd2, 0xbf,
  0xc8, 0x9e, 0x3f, 0x80, 0x32, 0x93, 0x9b, 0x8b, 0x08, 0x8d, 0x07, 0xf8,
  0x8e, 0xbb, 0xfb, 0x37, 0x43, 0x7d, 0x57, 0x19, 0x5d, 0x05, 0xc2, 0xab,
  0x79, 0x1b, 0x47, 0xbb, 0x88, 0xc5, 0x73, 0x47, 0x7a, 0xa6, 0x34, 0x13,
  0x8d, 0x3e, 0xc2, 0x27, 0x4f, 0xf4, 0x99, 0x7e, 0x49, 0x6e, 0x28, 0x59,
  0x13, 0x96, 0x7c, 0x65, 0xa7, 0x0d, 0x7c, 0x9b, 0x6e, 0x4f, 0x9c, 0x92,
  0x12, 0x04, 0xac, 0xa1, 0x02, 0x84, 0x1a, 0x76, 0xd0, 0x24, 0x84, 0xa5,
  0x1b, 0x90, 0xd3, 0x94, 0x46, 0xf1, 0xe6, 0x36, 0x7c, 0xd1, 0x30, 0xf6,
  0xe8, 0x16, 0x38, 0xf7, 0x2e, 0xdc, 0xed, 0x79, 0x8b, 0x73, 0x41, 0xd6,
  0xae, 0xea, 0xc4, 0xb8, 0x47, 0x6d, 0xdd, 0x0e, 0xed, 0xbd, 0xc2, 0xb0,
  0xbd, 0x3b, 0x3e, 0x56, 0x4e, 0x34, 0x28, 0x5a, 0xac, 0x9c, 0xed, 0x47,
  0x74, 0xa8, 0x06, 0x74, 0x5c, 0x61, 0x6f, 0xdd, 0x93, 0x42, 0xee, 0x83,
  0x00, 0xe1, 0x8e, 0xdd, 0xe8, 0x5a, 0xdd, 0x1d, 0xdc, 0xa1, 0xe1, 0xd6,
  0x1d, 0xa4, 0x52, 0xae, 0xe1, 0xc6, 0xa0, 0xa7, 0xe2, 0xbe, 0x23, 0x4a,
  0x06, 0xa3, 0xa4, 0xa5, 0x91, 0xf3, 0x17, 0x5b, 0xc7, 0xa7, 0x89, 0xb1,
  0x6c, 0x75, 0x7e, 0xdb, 0x6c, 0xb5, 0x8c, 0x41, 0xd9, 0x55, 0xc7, 0xe2,
  0xee, 0x08, 0x42, 0x49, 0xd1, 0x82, 0x6d, 0x10, 0xc8, 0x32, 0x0c, 0x04,
  0xca, 0xd1, 0xda, 0x4e, 0x27, 0xf0, 0xc3, 0x27, 0xcf, 0x73, 0x12, 0x28,
  0xa0, 0xf4, 0xb7, 0x9d, 0x8f, 0x60, 0x95, 0x64, 0x65, 0x5f, 0xfc, 0xe4,
  0xd2, 0x86, 0x0c, 0xdc, 0xac, 0x61, 0x40, 0xd1, 0xe9, 0x6a, 0x98, 0xc3,
  0xe5, 0x90, 0x7f, 0xef, 0x19, 0xb6, 0xf6, 0xcf, 0x02, 0xc3, 0x71, 0xb0,
  0xb8, 0x4f, 0xb2, 0x50, 0x82, 0x4e, 0x0f, 0x63, 0xb9, 0x97, 0x36, 0x27,
  0x67, 0xbb, 0xdf, 0x6d, 0xa1, 0x8c, 0x00, 0x9f, 0x9d, 0xca, 0x09, 0xbe,
  0xe1, 0x82, 0x73, 0x3d, 0x18, 0x59, 0xd7, 0x03, 0x29, 0x32, 0xa9, 0xcd,
  0x68, 0xc1, 0x1e, 0x0d, 0xe6, 0x64, 0x76, 0xa9, 0xec, 0xde, 0x08, 0xc8,
  0x2a, 0x27, 0xa1, 0x99, 0x14, 0xf0, 0x08, 0xc1, 0x14, 0xe0, 0x5e, 0x6b,
  0x60, 0x1d, 0x80, 0xeb, 0x0a, 0x2a, 0x6e, 0xf9, 0x45, 0xff, 0xae, 0xd3,
  0x38, 0x6b, 0xff, 0x0f, 0x6c, 0x91, 0x43, 0xc0, 0x8f, 0xfd, 0xe8, 0xc3,
  0xb3, 0x0f, 0xde, 0xaa, 0xa0, 0xc0, 0x2f, 0xb3, 0x57, 0xde, 0xb8, 0xf0,
  0x17, 0xfd, 0x01, 0xb3, 0x36, 0xe9, 0x11, 0x4c, 0x03, 0x00, 0x00 };

static const char PROGMEM spiffsformating_html[]  =  {
  0x1f, 0x8b, 0x08, 0x08, 0xd7, 0xce, 0x09, 0x5a, 0x02, 0x03, 0x66, 0x6f,
  0x72, 0x6d, 0x61, 0x74, 0x66, 0x73, 0x5f, 0x65, 0x78, 0x65, 0x2e, 0x68,
  0x74, 0x6d, 0x6c, 0x00, 0x4d, 0x53, 0x6b, 0x6f, 0xda, 0x30, 0x14, 0xfd,
  0x2b, 0x6e, 0xbe, 0xc4, 0x86, 0xe0, 0x00, 0x5d, 0xd9, 0x23, 0x0f, 0xa9,
  0xeb, 0x43, 0xab, 0xb6, 0x4a, 0xd5, 0x5a, 0x69, 0x1f, 0x28, 0x93, 0x8c,
  0x73, 0x43, 0x3c, 0x8c, 0x1d, 0x39, 0xa6, 0x8c, 0xe1, 0xfc, 0xf7, 0x39,
  0xa1, 0x65, 0x8b, 0x22, 0xe5, 0xe6, 0x9e, 0x7b, 0xcf, 0xb9, 0xc7, 0xb9,
  0x49, 0x2b, 0xbb, 0x91, 0x79, 0x5a, 0x01, 0x2b, 0xf2, 0xd4, 0x0a, 0x2b,
  0x21, 0x7f, 0x7c, 0xb8, 0xbb, 0xbd, 0x7d, 0x44, 0xb7, 0xda, 0x6c, 0x98,
  0x4d, 0xe3, 0x63, 0x32, 0x6d, 0xb8, 0x11, 0xb5, 0xcd, 0xe3, 0x41, 0x7a,
  0x36, 0xbf, 0xba, 0xbe, 0x7c, 0xba, 0x9c, 0x0f, 0x62, 0x78, 0x61, 0x12,
  0x97, 0x5b, 0xc5, 0xad, 0xd0, 0x0a, 0xd7, 0x11, 0x8b, 0x78, 0xb4, 0x8e,
  0x20, 0x32, 0xe4, 0x00, 0xd9, 0x29, 0xcf, 0xc9, 0xc1, 0x80, 0xdd, 0x1a,
  0x85, 0x38, 0xb5, 0xfa, 0xd1, 0x1a, 0xa1, 0x56, 0x98, 0x91, 0x36, 0x11,
  0x25, 0x3e, 0x0b, 0x43, 0x6a, 0xa0, 0x96, 0x8c, 0x03, 0x8e, 0x7f, 0xc6,
  0xd1, 0x11, 0x25, 0xe4, 0xb0, 0xab, 0x84, 0x04, 0xcc, 0x47, 0x23, 0x62,
  0xe6, 0xfe, 0x49, 0x16, 0xd9, 0x7a, 0xce, 0x17, 0xce, 0x75, 0x71, 0xb2,
  0xce, 0xe6, 0x27, 0x76, 0x38, 0xb1, 0xfb, 0xc2, 0x45, 0xbb, 0x48, 0xfe,
  0x53, 0x7e, 0x83, 0xc2, 0xe7, 0xe7, 0xdd, 0x30, 0x6c, 0x13, 0x9e, 0x4d,
  0xda, 0xe4, 0x1f, 0xb3, 0xd7, 0xef, 0x48, 0x49, 0x9d, 0xd5, 0xa7, 0x21,
  0x14, 0xec, 0xd0, 0x77, 0x58, 0xdd, 0xfc, 0xae, 0xb1, 0xef, 0x5a, 0x86,
  0xc3, 0x4e, 0x70, 0xd8, 0x87, 0x51, 0xb8, 0x0a, 0x49, 0xd4, 0x77, 0x24,
  0xaf, 0x92, 0x75, 0x8b, 0xc3, 0x29, 0x7a, 0xef, 0x85, 0xc6, 0x88, 0x65,
  0xe7, 0xf4, 0x1d, 0x26, 0xc9, 0x07, 0x3c, 0xed, 0xdf, 0x79, 0xf6, 0x91,
  0x16, 0x18, 0xf7, 0xc9, 0x11, 0x23, 0xf1, 0x05, 0x49, 0x3c, 0x57, 0x3a,
  0xeb, 0xb0, 0x65, 0x56, 0xd2, 0x15, 0x0e, 0xaa, 0x80, 0x24, 0x4b, 0x2a,
  0xb2, 0x20, 0x18, 0xe2, 0xd9, 0x88, 0x93, 0xf6, 0xd7, 0x61, 0x42, 0xd7,
  0xd9, 0x84, 0xca, 0x61, 0x10, 0xc7, 0xc1, 0x70, 0x42, 0x37, 0x6d, 0x1b,
  0x5d, 0xf8, 0xa3, 0x0a, 0xa3, 0xe9, 0x79, 0x77, 0x87, 0x2f, 0xcc, 0x38,
  0xa9, 0x39, 0xeb, 0xfc, 0xb9, 0x37, 0xa3, 0xee, 0x9a, 0x59, 0x70, 0x4a,
  0xef, 0xdc, 0x64, 0x3c, 0x1e, 0xbb, 0xd9, 0xd8, 0x71, 0xbd, 0x55, 0xd6,
  0x35, 0x60, 0xef, 0x94, 0x05, 0xe3, 0x3f, 0x93, 0xbb, 0x67, 0xb6, 0x72,
  0xfe, 0x32, 0x1e, 0x29, 0x9c, 0x28, 0x5d, 0xa1, 0xf9, 0x76, 0x03, 0xbe,
  0x6a, 0x05, 0xf6, 0x46, 0x42, 0x17, 0x7e, 0xde, 0xdf, 0x15, 0xbe, 0x89,
  0x3b, 0xa1, 0x14, 0x98, 0x2f, 0x4f, 0xf7, 0xdf, 0x1c, 0xc8, 0x06, 0x5c,
  0x65, 0xa0, 0x74, 0xb5, 0xd1, 0x56, 0x73, 0x2d, 0x5d, 0xa5, 0x1b, 0xab,
  0xd8, 0x06, 0x42, 0xda, 0xd4, 0x52, 0x58, 0x1c, 0x3a, 0x7f, 0x2c, 0xe3,
  0xe8, 0xd0, 0x12, 0x12, 0x0f, 0x16, 0x8b, 0x7c, 0x10, 0xa7, 0xf1, 0xeb,
  0xb6, 0xa4, 0xf1, 0x71, 0xb1, 0x96, 0xba, 0xd8, 0x23, 0xad, 0xa4, 0x66,
  0x45, 0xd6, 0x8f, 0x86, 0x49, 0xfe, 0x20, 0x81, 0x35, 0x80, 0x7e, 0x30,
  0x61, 0x29, 0xfa, 0x0a, 0x50, 0xa3, 0x2b, 0x26, 0x37, 0x88, 0xa9, 0x02,
  0x49, 0xb0, 0xc8, 0x56, 0x80, 0xca, 0x7e, 0x05, 0xad, 0xdf, 0x08, 0xc4,
  0x99, 0x31, 0x1d, 0x05, 0x45, 0xd7, 0x5a, 0x85, 0x1e, 0xd6, 0x5b, 0x5e,
  0xf9, 0xe2, 0xbd, 0xad, 0x3a, 0xd8, 0x53, 0x0a, 0xd9, 0xf7, 0xd4, 0x6c,
  0x05, 0xc8, 0x40, 0x27, 0x05, 0x05, 0x4d, 0x97, 0x26, 0x3f, 0x2e, 0xb2,
  0xaf, 0xa2, 0x94, 0xa2, 0xb4, 0xa9, 0x99, 0x42, 0xa2, 0xc8, 0x02, 0xef,
  0x33, 0x40, 0x8d, 0xdd, 0x4b, 0xc8, 0x82, 0x52, 0x2b, 0x3b, 0x6a, 0xc4,
  0x1f, 0xf8, 0x74, 0x3e, 0x0d, 0xf2, 0xd9, 0xd8, 0x1b, 0xf0, 0x65, 0x7e,
  0xfc, 0x6e, 0xf0, 0xce, 0x45, 0xf7, 0x93, 0xfc, 0x05, 0xf2, 0x28, 0x73,
  0xf0, 0x2b, 0x03, 0x00, 0x00
};

//holds the current upload
static File fsUploadFile;

static String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

static bool handleFileRead(String path){
  DBG_PRINTLN("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

static void handleFileUpload(void){
  if(server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    DBG_PRINT("handleFileUpload Name: "); DBG_PRINTLN(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //DBG_PRINT("handleFileUpload Data: "); DBG_PRINTLN(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    DBG_PRINT("handleFileUpload Size: "); DBG_PRINTLN(upload.totalSize);
  }
}

static void handleFileDelete(void){
  if(server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_PRINTLN("handleFileDelete: " + path);
  if(path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if(!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

static void handleFileCreate(void){
  if(server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  DBG_PRINTLN("handleFileCreate: " + path);
  if(path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if(SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if(file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

static void handleFileList(void) {
  if(!server.hasArg("dir")) {server.send(500, "text/plain", "BAD ARGS"); return;}

  String path = server.arg("dir");
  DBG_PRINTLN("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}
#endif

#if (DEVELOPMENT_OTA == true) || (DEVELOPMENT_FILEMANAGER == true)

void ESPUpdateServer_setup(const char* user, const char* pass){

#if DEVELOPMENT_FILEMANAGER == true
  //SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  server.on(FILE_MANAGEMENT_PATH, HTTP_GET, [](){
//    if(!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
	  server.sendHeader("Content-Encoding", "gzip");
	   server.send_P(200,"text/html",edit_htm_gz,edit_htm_gz_len);
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, [](){ server.send(200, "text/plain", ""); }, handleFileUpload);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, [](){
    String json = "{";
    json += "\"heap\":"+String(ESP.getFreeHeap());
    json += ", \"analog\":"+String(analogRead(A0));
    json += ", \"gpio\":"+String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });

  server.on(SPIFFS_FORMAT_PATH,HTTP_GET, [](){
     server.sendHeader("Content-Encoding", "gzip");
	    server.send_P(200,"text/html",spiffsformat_html,sizeof(spiffsformat_html));
  });
  server.on(SPIFFS_FORMATTING_PATH,HTTP_GET, [](){
      server.sendHeader("Content-Encoding", "gzip");
	    server.send_P(200,"text/html",spiffsformating_html,sizeof(spiffsformating_html));
      SPIFFS.format();      
  });

#endif

#if EXTERNALDATA_ON_SYNC_SERVER
     server.on("/gravity",HTTP_POST, [](){
        if (server.hasArg("plain")== false){ //Check if body received
            server.send(200, "text/plain", "");
            return;
        }
        uint8_t error;
        String json=server.arg("plain");
        char *data=(char*)malloc(json.length() +1);
        if(!data){
            server.send(500);
            return;
        }
        strcpy(data,json.c_str());
		if(externalData.processJSON(data,json.length(),false,error)){
    		server.send(200,"application/json","{}");
		}else{
		     server.send(500);
		}
     });
#endif


#if DEVELOPMENT_OTA == true
 // Flash update server
	httpUpdater.setup(&server,SYSTEM_UPDATE_PATH,user,pass);
#endif


  server.begin();
  DBG_PRINTLN("HTTP Update server started\n");

}


void ESPUpdateServer_loop(void){
  server.handleClient();
}

#endif
