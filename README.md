# Sports Arcade

An original football game project designed to grow from a quick browser-playable prototype into a deeper 3D game.

## Play in a browser

The repository now includes a self-contained browser version at **[index.html](./index.html)**. It uses HTML, CSS, and JavaScript, with no build tools or external game assets required.

### Controls

- **Computer:** WASD or arrow keys to move, Space to shoot, Shift to sprint, P to pause.
- **iPad / touch screen:** Drag on the left side of the pitch to move; use the SHOOT and SPRINT buttons below the pitch.
- **Goal:** Score against CPU United before the 90-second match ends.

### Publish with GitHub Pages

The browser game can be served as a static site with GitHub Pages. The repository is currently private; on GitHub Free, Pages generally requires a public repository. To publish for free, change the repository visibility to public (only if you are comfortable making the code public), then open **Settings → Pages** and select **GitHub Actions** as the build and deployment source. A workflow will publish the browser game when pushed to `main`.

## Godot 4 prototype

The project also includes an early procedural 3D starter:

- `project.godot`
- `main.tscn`
- `scripts/game.gd`

The Godot version is intended for a Mac or PC with Godot 4 installed. It has not yet been tested in a local Godot runtime. The browser game is the better option for playing on an iPad today.

## Roadmap

1. Test and refine the browser controls and gameplay on touch and desktop.
2. Add passing, tackles, smarter opponent AI, and goalkeeper saves.
3. Improve animations, audio, menus, teams, and match options.
4. Continue the separate 3D Godot version for desktop.

All teams, visual identity, and assets should remain original rather than copying proprietary FIFA / EA Sports FC branding or licensed player likenesses.
