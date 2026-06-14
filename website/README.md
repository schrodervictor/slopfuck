# slopfuck — website

Static marketing site for **slopfuck**, built with [Astro](https://astro.build).
Self-contained in this directory; no upstream project dependencies.

Canonical domain: `slopfuck.com`. Alternate: `slopfuck.org`.

## Develop

```sh
cd website
npm install
npm run dev
```

Local server at `http://localhost:4321`.

## Build

```sh
npm run build
```

Output goes to `dist/`. Deploy that to any static host — Cloudflare
Pages, Netlify, Vercel, GitHub Pages, S3 + CloudFront, fly.io, your
own nginx. The site has no runtime, no SSR, no API calls.

## Preview

```sh
npm run preview
```

## Structure

```
website/
├── astro.config.mjs       — Astro static-output config
├── package.json           — only dep is astro itself
├── tsconfig.json          — strict TS
├── public/
│   └── favicon.svg        — pilcrow ¶ in the brand gradient
└── src/
    ├── layouts/
    │   └── Base.astro     — HTML scaffold + OG/Twitter meta
    ├── components/
    │   ├── Nav.astro
    │   ├── Hero.astro
    │   ├── Features.astro
    │   ├── Comparison.astro
    │   ├── Testimonials.astro
    │   ├── Pricing.astro
    │   ├── CTA.astro
    │   └── Footer.astro
    ├── pages/
    │   └── index.astro    — composes the landing
    └── styles/
        └── global.css     — design tokens, reset, base typography
```

Each component uses Astro's scoped `<style>` block — no CSS framework
required, no Tailwind, no PostCSS plugins. Just plain CSS.

## Editing

To rewrite copy: each component holds its own content as an array of
objects in the component's frontmatter (e.g. `Features.astro` lists
its features, `Testimonials.astro` lists its quotes). Add or remove
entries to taste — the grids reflow automatically.

The design tokens (brand colors, spacing, radii, shadows) are CSS
custom properties in `src/styles/global.css`. Change them once and
the whole site follows.

## Notes on tone

The site commits, broadly speaking, to the bit. Every feature is
stated as a serious AI-first advantage. The testimonials are
attributed to fictional companies that read as plausible 2026
startup names. The pricing tiers escalate from `Free` to `Discovery`
to `Bespoke`. The footer signs off with *"Let me know if you'd like
me to dive deeper."*

If at any point during editing you feel the urge to add a wink, do
not. Not telling the joke is the funniest way to tell the joke.
