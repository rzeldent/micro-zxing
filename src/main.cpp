#include <Arduino.h>
#include <camera_config.h>

#include <ReadBarcode.h>
#include <ZXAlgorithms.h> // ToHex()

const auto options = ZXing::ReaderOptions()
                         .setFormats(ZXing::BarcodeFormat::All)
                         .setMaxNumberOfSymbols(1)
                         .setTryRotate(true)
                         .setTryHarder(true);

esp_err_t camera_init_result;

// put your setup code here, to run once:
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Inverted
  pinMode(LED_FLASH, OUTPUT);

  digitalWrite(LED_BUILTIN, false); // Off
  digitalWrite(LED_FLASH, false);   // Off

  Serial.begin(115200);
  while (!Serial)
    delay(10);

  camera_config_t camera_config = esp32cam_aithinker_settings;

  log_i("Camera configuration:");
  log_i("XCLK frequency: %d Hz", camera_config.xclk_freq_hz);
  log_i("Frame size: %d x %d", camera_config.frame_size, camera_config.frame_size);
  log_i("Pixel format: %d", camera_config.pixel_format);
  log_i("Frame buffer location: %d", camera_config.fb_location);
  log_i("Frame buffer count: %d", camera_config.fb_count);

  if (camera_config.fb_location == CAMERA_FB_IN_PSRAM && !psramInit())
    log_e("Failed to initialize PSRAM");

    // Only allow JPG as raw takes too much bandwith
      assert(camera_config.pixel_format == PIXFORMAT_JPEG);

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

  //Capture the frame as JPEG
  auto fb = esp_camera_fb_get();
  if (!fb)
  {
    log_e("Camera capture failed");
    return;
  }

  log_i("Captured JPEG image! Buffer size: %d bytes", fb->len);

  uint8_t *rgb_buffer = nullptr;
  size_t rgb_len;
  if (!frame2bmp(fb, &rgb_buffer, &rgb_len))
  {
    log_e("Frame to BMP(RGB888) conversion failed.");
    esp_camera_fb_return(fb);
    return;
  }

  log_i("Successfully converted to RGB888! Buffer size: %d bytes", rgb_len);
  ZXing::ImageView image_view(rgb_buffer, fb->width, fb->height, ZXing::ImageFormat::RGB);
  auto barcodes = ZXing::ReadBarcodes(image_view, options);
  free(rgb_buffer); // Free the RGB buffer after use

  log_i("Found %d barcodes", barcodes.size());
  for (auto const &barcode : barcodes)
  {
    log_i("Text:       \"%s\"", barcode.text().c_str());
    log_i("Bytes:      %s", ZXing::ToHex(options.textMode() == ZXing::TextMode::ECI ? barcode.bytesECI() : barcode.bytes()).c_str());
    log_i("Format:     %s", ZXing::ToString(barcode.format()).c_str());
    log_i("Identifier: %s", barcode.symbologyIdentifier().c_str());
    log_i("Content:    %s", ToString(barcode.contentType()).c_str());
    log_i("HasECI:     %d", barcode.hasECI());
    auto pos = barcode.position();
    log_i("Position:   TL=(%d,%d) TR=(%d,%d) BR=(%d,%d) BL=(%d,%d)", pos.topLeft().x, pos.topLeft().y, pos.topRight().x, pos.topRight().y, pos.bottomRight().x, pos.bottomRight().y, pos.bottomLeft().x, pos.bottomLeft().y);
    log_i("Rotation:   %d deg", barcode.orientation());
    log_i("IsMirrored: %d", barcode.isMirrored());
    log_i("IsInverted: %d", barcode.isInverted());
  }

  esp_camera_fb_return(fb); // Return the camera frame buffer to the pool
}