# Inset

A minimal Pebble watchface, neatly inset from the edge.

![Inset screenshot](screenshot.png)

Time and date are right-aligned, with a slim battery indicator on the right. Uses the watch’s 12/24-hour setting and shows the date as `Jun 12`.

Supports all Pebble models, with larger text on Time 2 and Round 2. No animations or network requests.

## Appearance

Dark mode is the default. Choose light or dark mode in Inset’s settings in the Pebble phone app, then tap Save. The watch remembers your choice. Monochrome models use a dashed indicator for used battery capacity; color models use light or dark gray.

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
