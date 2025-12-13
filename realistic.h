/**
 * realistic.h - Three-Valued Logic Implementation
 * 
 * Implements the Realistic Type from research paper:
 * "Implementing Three-Valued Logic with the Realistic Type:
 *  A Novel Approach to Uncertainty Handling in Programming Languages"
 * 
 * Organization: Daftyon
 * Location: Morocco
 * 
 * Truth Values:
 *   TRUE (1)      - Definitely true
 *   FALSE (0)     - Definitely false  
 *   REALISTIC (2) - Uncertain/Unknown/Possible
 * 
 * Based on Kleene's three-valued logic with extensions for
 * practical uncertainty handling in operating systems.
 */

#ifndef INCLUDE_REALISTIC_H
#define INCLUDE_REALISTIC_H

/** realistic_t: Three-valued logic type */
typedef enum {
    R_FALSE = 0,      /* Definitely false */
    R_TRUE = 1,       /* Definitely true */
    R_REALISTIC = 2   /* Uncertain/Unknown/Possible */
} realistic_t;

/** Logical Operations */

/** realistic_not: Logical NOT
 *  ¬TRUE = FALSE
 *  ¬FALSE = TRUE
 *  ¬REALISTIC = REALISTIC (uncertainty preserved)
 */
realistic_t realistic_not(realistic_t a);

/** realistic_and: Logical AND (Kleene's strong conjunction)
 *  TRUE ∧ TRUE = TRUE
 *  TRUE ∧ FALSE = FALSE
 *  TRUE ∧ REALISTIC = REALISTIC
 *  FALSE ∧ X = FALSE (short-circuit)
 *  REALISTIC ∧ REALISTIC = REALISTIC
 */
realistic_t realistic_and(realistic_t a, realistic_t b);

/** realistic_or: Logical OR (Kleene's strong disjunction)
 *  TRUE ∨ X = TRUE (short-circuit)
 *  FALSE ∨ FALSE = FALSE
 *  FALSE ∨ REALISTIC = REALISTIC
 *  REALISTIC ∨ REALISTIC = REALISTIC
 */
realistic_t realistic_or(realistic_t a, realistic_t b);

/** realistic_xor: Logical XOR
 *  TRUE ⊕ FALSE = TRUE
 *  FALSE ⊕ FALSE = FALSE
 *  TRUE ⊕ TRUE = FALSE
 *  X ⊕ REALISTIC = REALISTIC (if either is uncertain)
 */
realistic_t realistic_xor(realistic_t a, realistic_t b);

/** realistic_implies: Logical implication (→)
 *  FALSE → X = TRUE (vacuous truth)
 *  TRUE → TRUE = TRUE
 *  TRUE → FALSE = FALSE
 *  TRUE → REALISTIC = REALISTIC
 *  REALISTIC → X = REALISTIC
 */
realistic_t realistic_implies(realistic_t a, realistic_t b);

/** realistic_equiv: Logical equivalence (↔)
 *  TRUE ↔ TRUE = TRUE
 *  FALSE ↔ FALSE = TRUE
 *  TRUE ↔ FALSE = FALSE
 *  X ↔ REALISTIC = REALISTIC
 */
realistic_t realistic_equiv(realistic_t a, realistic_t b);

/** Utility Functions */

/** realistic_is_true: Check if definitely true */
int realistic_is_true(realistic_t a);

/** realistic_is_false: Check if definitely false */
int realistic_is_false(realistic_t a);

/** realistic_is_realistic: Check if uncertain */
int realistic_is_realistic(realistic_t a);

/** realistic_is_known: Check if value is known (TRUE or FALSE) */
int realistic_is_known(realistic_t a);

/** realistic_to_string: Convert to string for display */
const char* realistic_to_string(realistic_t a);

/** realistic_from_int: Convert integer to realistic_t
 *  0 → FALSE
 *  1 → TRUE
 *  2 → REALISTIC
 *  other → REALISTIC (unknown values treated as uncertain)
 */
realistic_t realistic_from_int(int value);

/** Advanced Operations (for uncertainty handling) */

/** realistic_resolve: Attempt to resolve REALISTIC to TRUE/FALSE
 *  Based on probability or additional information
 *  Returns TRUE with probability p, FALSE with (1-p)
 *  If value is already known, returns it unchanged
 */
realistic_t realistic_resolve(realistic_t a, unsigned int probability_percent);

/** realistic_probability: Get certainty level
 *  Returns 100 for TRUE/FALSE (certain)
 *  Returns 50 for REALISTIC (uncertain)
 */
unsigned int realistic_probability(realistic_t a);

#endif /* INCLUDE_REALISTIC_H */
