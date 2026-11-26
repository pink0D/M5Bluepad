//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/M5Bluepad
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "WebServer.h"
#include "ConfigComponent.h"

#include <esp_http_server.h>

/*void printHeapStats(const char* title) {
  Serial.printf("%s\n", title);
  Serial.printf("Total Heap: %u bytes\n", ESP.getHeapSize());
  Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
  Serial.printf("Min Free Heap (low water mark): %u bytes\n", ESP.getMinFreeHeap());
  Serial.printf("Max Allocatable Block: %u bytes\n", ESP.getMaxAllocHeap());

  // Optional: If your board has external PSRAM
  if (psramFound()) {
    Serial.printf("Total PSRAM: %u bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %u bytes\n", ESP.getFreePsram());
  }
}*/

class HttpdChunkPrint : public Print {
private:
    httpd_req_t* _req;
    char *_buffer; // Fixed array size - heap allocated!
    size_t _pos = 0;
    esp_err_t _err = ESP_OK;

public:
    HttpdChunkPrint(httpd_req_t* req) : _req(req) {
        _buffer = new char[512];
    }

    ~HttpdChunkPrint() {
        delete [] _buffer;
    }

    size_t write(uint8_t c) override {
        if (_err != ESP_OK) return 0;

        _buffer[_pos++] = (char)c;
        if (_pos >= sizeof(_buffer)) {
            flush();
            if (_err != ESP_OK) return 0;
        }
        return 1;
    }

    size_t write(const uint8_t* buffer, size_t size) override {
        if (_err != ESP_OK) return 0;

        size_t written = 0;
        while (written < size) {
            size_t space = sizeof(_buffer) - _pos;
            size_t toCopy = (size - written < space) ? (size - written) : space;
            
            memcpy(_buffer + _pos, buffer + written, toCopy);
            _pos += toCopy;
            written += toCopy;

            if (_pos >= sizeof(_buffer)) {
                flush();
                if (_err != ESP_OK) return written;
            }
        }
        return written;
    }

    void flush() override {
        if (_pos > 0 && _err == ESP_OK) {
            _err = httpd_resp_send_chunk(_req, _buffer, _pos);
            _pos = 0;
        }
    }

    esp_err_t getResult() const { return _err; }
};


namespace bluepadhub::web {    

    bool WebServer::start() {

        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        //config.stack_size = 16*1024;
        //config.lru_purge_enable = true;
        //config.send_wait_timeout = 15;
        //config.recv_wait_timeout = 15;
        config.max_uri_handlers = 16;
        config.uri_match_fn = httpd_uri_match_wildcard;

        httpd_uri_t root_config_uri = {
            .uri = "/config",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->root_config_handler(req);
            },
            .user_ctx = this,
        };  

        httpd_uri_t component_config_get_uri = {
            .uri = "/config/*",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->component_config_handler_get(req);
            },
            .user_ctx = this,
        };  

        httpd_uri_t component_config_post_uri = {
            .uri = "/config/*",
            .method = HTTP_POST,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->component_config_handler_post(req);
            },
            .user_ctx = this,
        };  

        httpd_uri_t component_config_options_uri = {
            .uri = "/config/*",
            .method = HTTP_OPTIONS,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->component_config_handler_options(req);
            },
            .user_ctx = this,
        };  

        httpd_uri_t files_uri = {
            .uri = "/*",
            .method = HTTP_GET,
            .handler = [](httpd_req_t *req) -> esp_err_t {
                return static_cast<WebServer*>(req->user_ctx)->file_handler(req);
            },
            .user_ctx = this,
        };  

        config.server_port = 80;
        config.ctrl_port = 30080;

        Serial.print("Starting web server on port ");
        Serial.println(config.server_port);
        
        if (httpd_start(&web_httpd, &config) == ESP_OK) {
            httpd_register_uri_handler(web_httpd, &root_config_uri);
            httpd_register_uri_handler(web_httpd, &component_config_get_uri);
            httpd_register_uri_handler(web_httpd, &component_config_post_uri);
            httpd_register_uri_handler(web_httpd, &component_config_options_uri);
            httpd_register_uri_handler(web_httpd, &files_uri);
        }
        
        Serial.println("Web server started");

        return true;
    }

    esp_err_t WebServer::httpd_resp_send_json_chunk(httpd_req_t *req, JsonDocument &json) {

        updateActivity();

        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        httpd_resp_set_hdr(req, "Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
        //httpd_resp_set_hdr(req, "Connection", "close");

        HttpdChunkPrint writer(req);
        serializeJsonPretty(json, writer);
        writer.flush();

        esp_err_t res = writer.getResult();

        if (res != ESP_OK) {
            return res;
        }

        res = httpd_resp_send_chunk(req, nullptr, 0);

        //int sockfd = httpd_req_to_sockfd(req);
        //httpd_sess_trigger_close(req->handle, sockfd);

        return res;

        /*size_t jsonLength = measureJsonPretty(json);
        char* buffer = new char[jsonLength + 1];

        serializeJsonPretty(json, buffer, jsonLength + 1);

        esp_err_t res = httpd_resp_send_chunk(req, buffer, jsonLength);
        httpd_resp_send_chunk(req, nullptr, 0);*/

        /*const size_t chunk_size = 2048;
        size_t offset = 0;

        esp_err_t res= ESP_OK;

        while (offset < jsonLength) {
            size_t to_send = (jsonLength - offset > chunk_size) ? chunk_size : (jsonLength - offset);
            esp_err_t err = httpd_resp_send_chunk(req, buffer + offset, to_send);
            if (err != ESP_OK) {
                printf("Error: %s (0x%x)\n", esp_err_to_name(err), err);
                break;
            }
            offset += to_send;
        }
        httpd_resp_send_chunk(req, NULL, 0);  */      

        //delete [] buffer;

        //return res;
    }

    void WebServer::addWebFiles(const web_data &files) {

        webData.push_back(files);
    }

    esp_err_t WebServer::file_handler(httpd_req_t *req) {

        for (const auto& web : webData) {
            
            for (int i=0; i<web.count; i++) {

                if (strcmp(req->uri, web.files[i].file_name) == 0) {

                    httpd_resp_set_type(req, web.files[i].content_type);

                    if (web.files[i].content_encoding != nullptr)
                        httpd_resp_set_hdr(req, "Content-Encoding", web.files[i].content_encoding);

                    httpd_resp_send_chunk(req, (const char*)web.files[i].content, web.files[i].content_size);
                    httpd_resp_send_chunk(req, nullptr, 0);

                    return ESP_OK;
                }
            }         
        }

        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
   
    esp_err_t WebServer::root_config_handler(httpd_req_t *req) {

        JsonDocument json;

        json["settings_save_redirect"] = false;

        JsonArray jsonComponents = json["components"].to<JsonArray>();
        
        auto callback = [&jsonComponents](ConfigComponent* comp) {
            // list only top-level components
            if (comp->isExtension() == false) {
                JsonObject jsonComp = jsonComponents.add<JsonObject>();
                jsonComp["name"] = comp->getName();
                jsonComp["label"] = comp->getLabel();
                jsonComp["enabled"] = comp->isEnabled();
            }
        };        

        ConfigComponent::iterateComponents(callback);

        return httpd_resp_send_json_chunk(req, json);
    }

    esp_err_t WebServer::component_config_handler_options(httpd_req_t *req) {

        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
        httpd_resp_set_hdr(req, "Access-Control-Max-Age", "86400");

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_send(req, NULL, 0);

        return ESP_OK;
    }

    esp_err_t WebServer::component_config_handler_get(httpd_req_t *req) {

        // extract component name from '/config/<name>' uri
        std::string name = std::string(req->uri).substr(8); 

        ConfigComponent *component = ConfigComponent::getConfigComponent(name);

        if (component != nullptr) {

            JsonDocument json = component->saveSettingsToJson();

            return httpd_resp_send_json_chunk(req, json);
        }

        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    esp_err_t WebServer::component_config_handler_post(httpd_req_t *req) {

        // extract component name from '/config/<name>' uri
        std::string name = std::string(req->uri).substr(8); 

        ConfigComponent *component = ConfigComponent::getConfigComponent(name);

        // read JSON from request content
        size_t total_len = req->content_len;
        char *buffer = new char[total_len+1];

        esp_err_t res = httpd_req_recv(req, buffer, total_len);
        buffer[total_len] = '\0';

        if (res > 0) {

            JsonDocument json;
            DeserializationError error = deserializeJson(json, buffer);

            delete [] buffer;

            if ( (!error) && (component != nullptr) ) {

                JsonDocument resp;

                resp["success"] = component->updateSettings(json);

                return httpd_resp_send_json_chunk(req, resp);
            }
        } else {

            delete [] buffer;
        }
        
        if (res != ESP_OK)
            httpd_resp_send_500(req);

        return res;
    }

    void WebServer::updateActivity() {
        lastActivityTime = esp_timer_get_time();
    }

    long WebServer::getLastActivityTime() {
        return lastActivityTime;
    }

}