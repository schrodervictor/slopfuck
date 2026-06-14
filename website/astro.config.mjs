import { defineConfig } from 'astro/config';

// https://astro.build/config
export default defineConfig({
  output: 'static',
  // Canonical: slopfuck.com. The .org alternate is held for
  // foundational redundancy and broadly speaking serves the same
  // purpose.
  site: 'https://slopfuck.com',
  build: {
    inlineStylesheets: 'auto',
  },
});
