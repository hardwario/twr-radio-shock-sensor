<a href="https://www.hardwario.com/"><img src="https://www.hardwario.com/ci/assets/hw-logo.svg" width="200" alt="HARDWARIO Logo" align="right"></a>

# Radio Shock Sensor using accelerometer

[![Travis](https://img.shields.io/travis/bigclownlabs/bcf-skeleton/master.svg)](https://travis-ci.org/bigclownlabs/bcf-skeleton)
[![Release](https://img.shields.io/github/release/bigclownlabs/bcf-skeleton.svg)](https://github.com/bigclownlabs/bcf-skeleton/releases)
[![License](https://img.shields.io/github/license/bigclownlabs/bcf-skeleton.svg)](https://github.com/bigclownlabs/bcf-skeleton/blob/master/LICENSE)
[![Twitter](https://img.shields.io/twitter/follow/BigClownLabs.svg?style=social&label=Follow)](https://twitter.com/BigClownLabs)

Shock detector is using LIS2DH accelerometer on the Core Module. MCU is sleeping and sensor is sampling in low power mode. When the shock threshold is reached the interrupt wakes up processor and sends the MQTT message on the Raspberry Pi.

If you want to get more information about Core Module, firmware and how to work with it, please follow this link:

**https://www.bigclown.com/doc/firmware/basic-overview/**

User's application code (business logic) goes into `app/application.c`.
The default content works as a *Hello World* example.
When flashed into Core Module, it toggles LED state with each button press.

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT/) - see the [LICENSE](LICENSE) file for details.

---

Made with &#x2764;&nbsp; by [**HARDWARIO a.s.**](https://www.hardwario.com/) in the heart of Europe.
