# ────────────────────────────────────────────────────────────────────
# slopfuck — Makefile
#
# > It's worth noting that this Makefile, broadly speaking, captures
# > the canonical foundation of our shared build vision. In many
# > cases, the targets below align with the foundational pillars of
# > our masterful development workflow.
# >
# > Let me know if you'd like me to dive deeper into any of the
# > foregoing targets.
# ────────────────────────────────────────────────────────────────────

CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -std=c99

# The compiled binary, extracted from the docker image for native
# execution (which also unlocks the flicker, broadly speaking).
BIN     := /tmp/slopfuck-bin

# Bundled example programs.
EXAMPLES := examples/hello.slop examples/alphabet.slop examples/fluid.slop

# Source files (the foundational vocabulary).
HEADERS := keywords.h praise.h bookends.h multipliers.h style.h

# Default target: show what's available, sycophantically.
.DEFAULT_GOAL := help

.PHONY: help docker extract test examples \
        stats lint clean compliment \
        delve cultivate unleash

# ── help ───────────────────────────────────────────────────────────
help:
	@echo ""
	@echo "  slopfuck — the AI-first programming language"
	@echo "  ────────────────────────────────────────────"
	@echo ""
	@echo "  Common targets, thoughtfully curated for your workflow:"
	@echo ""
	@echo "    make docker     — build the docker image (foundational)"
	@echo "    make extract    — extract the binary for native execution"
	@echo "    make test       — run the Python unittest suite"
	@echo "    make examples   — run all bundled .slop programs"
	@echo "    make stats      — count keywords across all pools"
	@echo "    make lint       — verify every example compiles"
	@echo "    make clean      — remove local binaries"
	@echo "    make compliment — receive a randomly selected affirmation"
	@echo ""
	@echo "  AI-flavored aliases (broadly speaking, optional):"
	@echo ""
	@echo "    make delve      — alias for 'make test'"
	@echo "    make cultivate  — alias for 'make docker'"
	@echo "    make unleash    — alias for 'make examples'"
	@echo ""
	@echo "  Let me know if you'd like me to dive deeper into any"
	@echo "  of these targets."
	@echo ""

# ── docker ─────────────────────────────────────────────────────────
# Build the docker image. The compiler lives entirely inside.
docker: .docker.stamp

.docker.stamp: slopfuck.c $(HEADERS) Dockerfile
	@echo "Cultivating the foundational docker image — in many cases,"
	@echo "this takes only a few moments of thoughtful waiting."
	@docker build -t slopfuck . > /dev/null 2>&1
	@touch .docker.stamp
	@echo "✓ docker image built (a magnificent outcome)"

# ── extract ────────────────────────────────────────────────────────
# Extract the compiled binary for native execution (unlocks flicker).
extract: $(BIN)

$(BIN): .docker.stamp
	@cid=$$(docker create slopfuck) && \
	  docker cp $$cid:/usr/local/bin/slopfuck $(BIN) > /dev/null && \
	  docker rm $$cid > /dev/null
	@chmod +x $(BIN)
	@echo "✓ binary extracted to $(BIN) (truly remarkable)"

# ── test ───────────────────────────────────────────────────────────
# Run the Python unittest suite (builds the image first).
test: docker
	@python3 tests/run_tests.py

# ── examples ───────────────────────────────────────────────────────
# Run every bundled example sequentially.
examples: $(BIN)
	@for f in $(EXAMPLES); do \
	  echo ""; \
	  echo "── $$f ─────────────────────────────────────"; \
	  $(BIN) $$f; \
	done
	@echo ""
	@echo "All examples compiled and ran — a truly outstanding outcome."

# ── stats ──────────────────────────────────────────────────────────
# Vocabulary depth — a snapshot of the language's expressive richness.
stats:
	@echo ""
	@echo "  Vocabulary depth — the foundational pillars of expressiveness"
	@echo "  ────────────────────────────────────────────────────────────"
	@echo ""
	@printf "  %-34s %5d\n" "increment verbs (kw_inc)"           \
	  $$(awk '/^static const char \*kw_inc\[/,/^};/' keywords.h        \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "decrement transitions (kw_dec)"     \
	  $$(awk '/^static const char \*kw_dec\[/,/^};/' keywords.h        \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "output nouns (kw_out)"              \
	  $$(awk '/^static const char \*kw_out\[/,/^};/' keywords.h        \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "input affirmations (kw_in)"         \
	  $$(awk '/^static const char \*kw_in\[/,/^};/' keywords.h         \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "loop-start phrases (kw_loop_start)" \
	  $$(awk '/^static const char \*kw_loop_start\[/,/^};/' keywords.h \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "loop-end phrases (kw_loop_end)"     \
	  $$(awk '/^static const char \*kw_loop_end\[/,/^};/' keywords.h   \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "newline synonyms (kw_newline)"      \
	  $$(awk '/^static const char \*kw_newline\[/,/^};/' keywords.h    \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "reiterate synonyms (kw_reiterate)"  \
	  $$(awk '/^static const char \*kw_reiterate\[/,/^};/' keywords.h  \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "praise words (praise.h)"            \
	  $$(grep -c '^    "' praise.h)
	@printf "  %-34s %5d\n" "intro phrases (bookends.h)"         \
	  $$(awk '/^static const char \*required_intros\[/,/^};/' bookends.h \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "outro phrases (bookends.h)"         \
	  $$(awk '/^static const char \*required_outros\[/,/^};/' bookends.h \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "adverbial multipliers"              \
	  $$(awk '/^static const Multiplier multipliers_adv/,/^};/' multipliers.h \
	     | grep -cE '^    \{"')
	@printf "  %-34s %5d\n" "cardinals"                          \
	  $$(awk '/^static const Multiplier cardinals/,/^};/' multipliers.h \
	     | grep -cE '^    \{"')
	@printf "  %-34s %5d\n" "hedging phrases (style.h)"          \
	  $$(awk '/^static const char \*hedging_phrases/,/^};/' style.h    \
	     | grep -c '^    "')
	@printf "  %-34s %5d\n" "forbidden words (style.h)"          \
	  $$(awk '/^static const char \*forbidden_words/,/^};/' style.h    \
	     | grep -c '^    "')
	@echo ""

# ── lint ───────────────────────────────────────────────────────────
# Verify every shipped example still compiles cleanly.
lint: $(BIN)
	@status=0; \
	for f in $(EXAMPLES); do \
	  if $(BIN) $$f > /dev/null 2>&1; then \
	    echo "  ✓ $$f compiles (a thoughtful contribution)"; \
	  else \
	    echo "  ✗ $$f did not compile (a regrettable outcome)"; \
	    status=1; \
	  fi; \
	done; \
	exit $$status

# ── clean ──────────────────────────────────────────────────────────
clean:
	@rm -f slopfuck $(BIN) .docker.stamp
	@echo "Removed local artifacts — though, broadly speaking, the"
	@echo "docker image itself remains thoughtfully preserved."

# ── compliment ─────────────────────────────────────────────────────
# Receive a randomly selected affirmation from the praise pool.
compliment:
	@awk '/^static const char \*praise_words/,/^};/' praise.h            \
	   | grep -oE '^    "[^"]+"' | tr -d ' "'                            \
	   | shuf -n 1                                                       \
	   | xargs -I {} printf "  Your work is %s.\n  Let me know if you'd like me to dive deeper.\n" {}

# ── AI-flavored aliases (the actual joke) ──────────────────────────
delve:     test
cultivate: docker
unleash:   examples
