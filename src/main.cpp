#include <Arduino.h>
#include <camera_config.h>

#include <ReadBarcode.h>

esp_err_t camera_init_result;

// put your setup code here, to run once:
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Inverted
  pinMode(LED_FLASH, OUTPUT);

  digitalWrite(LED_BUILTIN, true); // Off
  digitalWrite(LED_FLASH, false);  // Off

  camera_config_t camera_config;
  memcpy(&camera_config, &esp32cam_aithinker_settings, sizeof(camera_config_t));
  camera_config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  camera_config.frame_size = framesize_t::FRAMESIZE_SVGA;
  camera_config.pixel_format = pixformat_t::PIXFORMAT_GRAYSCALE;

  camera_init_result = esp_camera_init(&camera_config);
  if (camera_init_result != ESP_OK)
  {
    log_e("Camera initialization failed. Error code: 0x%04x", camera_init_result);
    return;
  }

  log_i("Camera initialized!");
}

// put your main code here, to run repeatedly:
void loop()
{
  if (camera_init_result != ESP_OK)
    return;

  auto hints = ZXing::DecodeHints()
                   .setFormats(ZXing::BarcodeFormat::Any)
                   .setMaxNumberOfSymbols(1)
                   .setTryRotate(true)
                   .setTryHarder(true);

  auto fb = esp_camera_fb_get();
  ZXing::ImageView image(fb->buf, fb->width, fb->height, ZXing::ImageFormat::Lum);
  auto results = ZXing::ReadBarcodes(image, hints);
  esp_camera_fb_return(fb);

  for (auto const &result : results)
  {
    log_i("Text:       \"%s\"", result.text().c_str());
    log_i("Bytes:      %s", ZXing::ToHex(hints.textMode() == ZXing::TextMode::ECI ? result.bytesECI() : result.bytes()).c_str());
    log_i("Format:     %s", ZXing::ToString(result.format()).c_str());
    log_i("Identifier: %s", result.symbologyIdentifier().c_str());
    log_i("Content:    %s", ToString(result.contentType()).c_str());
    log_i("HasECI:     %d", result.hasECI());
    log_i("Position:   %d", result.position());
    log_i("Rotation:   %d deg", result.orientation());
    log_i("IsMirrored: %d", result.isMirrored());
    log_i("IsInverted: %d", result.isInverted());
  }
}