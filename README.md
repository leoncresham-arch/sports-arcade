# Sports Arcade

An original football game project designed to grow from a quick browser-playable prototype into a deeper 3D game.

## Play in a browser

The repository now includes a self-contained browser version at **[index.html](./index.html)**. It uses HTML, CSS, and JavaScript, with no build tools or external game assets required.

### Controls

- **Computer:** WASD or arrow keys to move, Space to shoot, E to pass, F to tackle, Shift to sprint, P to pause, R to restart.
- **iPad / touch screen:** Drag on the left side of the pitch to move; use the SHOOT, PASS, TACKLE, and SPRINT buttons below the pitch.
- **Goal:** Score against CPU United before the 90-second match ends. Both goals have moving keepers; a supporting teammate offers passing options; tackling can win the ball from the CPU; a stamina meter tracks sprint energy.

### Publish with GitHub Pages

The browser game can be served as a static site with GitHub Pages. The repository is public. To enable hosting, open **Settings → Pages** and select **GitHub Actions** as the build and deployment source. A workflow will publish the browser game when pushed to `main`.

## Godot 4 prototype

The project also includes an early procedural 3D starter:

- `project.godot`
- `main.tscn`
- `scripts/game.gd`

The Godot version is intended for a Mac or PC with Godot 4 installed. It has not yet been tested in a local Godot runtime. The browser game is the better option for playing on an iPad today.

## Product vision — premium football experience

Sports Arcade aims for a polished, original, 3D football experience with the responsive controls, visual quality, tactical depth, and broadcast presentation players expect from top football games. This is an ambition, **not a claim that the current prototype has AAA-level quality**.

**Primary platforms:** Safari and Chrome on iPad and desktop. Hardware performance, browser graphics support, touch controls, and download sizes are first-class constraints.

### Visual quality is the first priority

The current 2D web build is a temporary mechanics prototype. The primary next deliverable is a **high-quality 3D visual vertical slice** with a broadcast camera, believable player proportions and animation, professional pitch materials, stadium, kits, shadows, and lighting. Target modern premium football presentation while using wholly original assets and identities.

**Graphics-first order of work:**

1. Select a browser-compatible 3D rendering pipeline and establish a performance budget for iPad Safari and desktop browsers.
2. Create one carefully lit 3D stadium and realistic-looking pitch with camera controls.
3. Integrate a high-quality legally usable player model and football, with original kit materials and animation transitions.
4. Tune football-specific animation: run, sprint, turn, idle, dribble, pass, shoot, tackle, and keeper movement.
5. Add presentation polish: broadcast camera moves, UI, crowd, audio, replay and goal sequence.
6. Reintroduce and refine playable match systems using the 3D presentation, expanding only after testing quality and performance.

Keep the existing playable 2D build online during the transition. Avoid promising fidelity equivalent to a commercial AAA release until the 3D slice proves it achievable within budget and browser hardware constraints.

### Development milestones

1. **Playable core:** prototype passing, tackling and teammate support are implemented; next refine possession, passing accuracy, responsive controls, goalkeepers, scoring, and restart behavior with real touch and keyboard tests.
2. **3D vertical slice:** one attractive stadium, two original teams, a full pitch, smooth player locomotion, physically convincing ball movement, and camera options.
3. **Match systems:** gradually expand from small-sided play to 11v11, formations, switching players, referee rules, fouls, set pieces, substitutes, and capable team AI.
4. **Presentation:** original kits and player models, animation blending, goal celebrations, replays, menus, stadium sound, crowd ambience, accessible controls, and performance settings.
5. **Modes and progression:** quick matches, tournaments, leagues, then optional career-style features.
6. **Polish and release:** playtesting, difficulty tuning, responsive iPad layout, cross-browser compatibility, performance profiling, bug fixing, and release checks.

### Production principles

- Keep the existing lightweight browser prototype playable while the premium 3D game develops.
- Build and measure one high-quality 3D **vertical slice** before attempting a large roster of teams or game modes.
- Use original names, logos, kits, sounds, artwork, and other assets; do not copy licensed FIFA / EA Sports FC content.
- No feature is considered complete until it works reliably on supported browsers and input devices.
- A FIFA-scale feature set may require significant time, art production, budget, and testing; prioritize quality over quantity.

All teams, visual identity, and assets should remain original rather than copying proprietary FIFA / EA Sports FC branding or licensed player likenesses.
