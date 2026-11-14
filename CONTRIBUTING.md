# Contributing

Thanks for your interest in the project!

1. Fork the repo and create a branch: `git checkout -b feature/my-change`
2. Make your change and confirm it builds: `pio run`
3. Test on real hardware if you touched sensor or Wi-Fi code, and say which sensors you used.
4. Commit with a clear message (for example `Add SD logging`) and open a pull request describing what changed and why.

## Guidelines

- Keep the main loop non-blocking (avoid long `delay()` calls).
- Never commit Wi-Fi credentials or personal locations.
- Report bugs via Issues with your board, sensor models and serial output.
