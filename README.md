# Inset

A minimal Pebble watchface, neatly inset from the edge.

![Inset screenshot](screenshot.png)

Time and date are right-aligned, with a slim battery indicator on the right. Uses the watch’s 12/24-hour setting and shows the date as `Jun 12`.

Status icons beside the date appear when Bluetooth is disconnected or Quiet Time is active.

Light by default, with dark mode and optional disconnect vibration available in the Pebble app settings. Supports all Pebble models, with larger text on Time 2 and Round 2.

## Inspiration

Inset’s design is inspired by Simple, which was inspired by Blotch and Marked 2.

## Build & install

Requires the [Pebble SDK](https://developer.rebble.io/docs/).

```sh
pebble build
pebble install --cloudpebble
```

## Development

Built with assistance from Codex.

## License

[MIT](LICENSE) © 2026 Alexander Teinum.
