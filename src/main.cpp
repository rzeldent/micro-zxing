#include <Arduino.h>
#include <camera_config.h>

#include <ReadBarcode.h>

esp_err_t camera_init_result;

// put your setup code here, to run once:
void setup()
{

  camera_config_t camera_config;
  memcpy(&camera_config, &esp32cam_aithinker_settings, sizeof(camera_config_t));
  camera_config.frame_size = framesize_t::FRAMESIZE_QVGA;
  camera_config.pixel_format = pixformat_t::PIXFORMAT_GRAYSCALE;

  camera_init_result = esp_camera_init(&camera_config);
  if (camera_init_result != ESP_OK)
  {
    log_e("Camera initialization failed. Error code: 0x%4x", camera_init_result);
    return;
  }
}

void loop()
{
  if (camera_init_result != ESP_OK)
    return;

  ZXing::DecodeHints hints;

  // put your main code here, to run repeatedly:
  auto fb = esp_camera_fb_get();

  ZXing::ImageView image(fb->buf, fb->width, fb->height, ZXing::ImageFormat::Lum);
  auto results = ZXing::ReadBarcodes(image, hints);
  if (!results.empty())
  {
    for (auto const &result:results)
    {
      auto bytes = result.bytes();
      std::string text(reinterpret_cast<char const*>(bytes.data()), bytes.size());
      log_i("Detected: %s", text.c_str());
    }
  }

  esp_camera_fb_return(fb);

  delay(100);
}