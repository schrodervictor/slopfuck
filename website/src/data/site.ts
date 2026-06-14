// Centralised URLs and references for the slopfuck site.
//
// Editing this file updates every component that references the
// repo, branch, or canonical artefact paths.

export const GITHUB_REPO = 'https://github.com/schrodervictor/slopfuck';
export const GITHUB_BRANCH = 'canonical-foundation-of-our-shared-vision';
export const GITHUB_BLOB = `${GITHUB_REPO}/blob/${GITHUB_BRANCH}`;
export const GITHUB_TREE = `${GITHUB_REPO}/tree/${GITHUB_BRANCH}`;

// Canonical documents on the repository.
export const URL_DESIGN = `${GITHUB_BLOB}/DESIGN.md`;
export const URL_IDEAS = `${GITHUB_BLOB}/IDEAS.md`;
export const URL_LICENSE = `${GITHUB_BLOB}/LICENSE`;
export const URL_README = `${GITHUB_BLOB}/README.md`;
export const URL_TESTS = `${GITHUB_TREE}/tests`;
export const URL_EXAMPLES = `${GITHUB_TREE}/examples`;

// Contact.
export const EMAIL_HELLO = 'hello@slopfuck.com';
export const EMAIL_SUPPORT = 'support@slopfuck.com';
export const EMAIL_PRESS = 'press@slopfuck.com';

// Internal routes.
export const ROUTE_ABOUT = '/about';
export const ROUTE_VISION = '/vision';
export const ROUTE_PRESS = '/press';
export const ROUTE_DOCS = '/docs';
