// Documentation sections — central source of truth for the docs
// sidebar, the docs landing grid, and any cross-links. Edit a single
// entry here to update every surface that references it.

export type DocStatus = 'available' | 'forthcoming';

export interface DocSection {
  slug: string;
  title: string;
  summary: string;
  status: DocStatus;
}

export const DOC_SECTIONS: DocSection[] = [
  {
    slug: 'getting-started',
    title: 'Getting started',
    status: 'available',
    summary:
      'A thoughtful walkthrough from zero to your first compiled slopfuck program.',
  },
  {
    slug: 'reference',
    title: 'Language reference',
    status: 'available',
    summary:
      'A complete enumeration of operators, keywords, validators, and compile-time conventions.',
  },
  {
    slug: 'examples',
    title: 'Worked examples',
    status: 'available',
    summary:
      'A curated collection of idiomatic slopfuck programs, annotated for clarity.',
  },
  {
    slug: 'style-guide',
    title: 'Style guide',
    status: 'available',
    summary:
      'Conventions for inline string embedding, the pilcrow newline op, multipliers, bullets, and reiterate.',
  },
  {
    slug: 'compiler',
    title: 'Compiler architecture',
    status: 'available',
    summary:
      'The validators, the compile-time expansion model, and how to extend the language without abandoning brainfuck.',
  },
  {
    slug: 'debugging',
    title: 'Debugging',
    status: 'available',
    summary:
      'The agent-facing debug surface — <code>--stripped</code> collapses the prose, <code>--opcodes</code> renders the canonical brainfuck stream. A token-efficient inspection layer for machine reviewers.',
  },
  {
    slug: 'agent-integration',
    title: 'Agent integration',
    status: 'available',
    summary:
      'Patterns for guiding autonomous coding agents to produce idiomatic slopfuck on the first attempt.',
  },
  {
    slug: 'enterprise',
    title: 'Enterprise deployment',
    status: 'forthcoming',
    summary:
      'Considerations for organisations rolling slopfuck out at scale: praise-density policy, compile-time gating, and the canonical alignment review.',
  },
  {
    slug: 'faq',
    title: 'FAQ',
    status: 'available',
    summary:
      'Thoughtful answers to the questions our customers most often bring to our alignment partners.',
  },
];

export const DOCS_BASE = '/docs';

export function docHref(slug: string): string {
  return `${DOCS_BASE}/${slug}`;
}
