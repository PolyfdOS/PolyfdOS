/**
 * realistic.c - Three-Valued Logic Implementation
 * 
 * Based on Kleene's strong three-valued logic with practical
 * extensions for operating system uncertainty handling.
 */

#include "realistic.h"

/** Truth tables for three-valued logic operations */

/* NOT truth table:
 * T → F
 * F → T  
 * R → R
 */
static const realistic_t NOT_TABLE[3] = {
    R_TRUE,      /* ¬FALSE = TRUE */
    R_FALSE,     /* ¬TRUE = FALSE */
    R_REALISTIC  /* ¬REALISTIC = REALISTIC */
};

/* AND truth table (Kleene's strong conjunction):
 *     F  T  R
 * F   F  F  F
 * T   F  T  R
 * R   F  R  R
 */
static const realistic_t AND_TABLE[3][3] = {
    /* FALSE */ {R_FALSE, R_FALSE, R_FALSE},
    /* TRUE  */ {R_FALSE, R_TRUE,  R_REALISTIC},
    /* REAL. */ {R_FALSE, R_REALISTIC, R_REALISTIC}
};

/* OR truth table (Kleene's strong disjunction):
 *     F  T  R
 * F   F  T  R
 * T   T  T  T
 * R   R  T  R
 */
static const realistic_t OR_TABLE[3][3] = {
    /* FALSE */ {R_FALSE, R_TRUE, R_REALISTIC},
    /* TRUE  */ {R_TRUE,  R_TRUE, R_TRUE},
    /* REAL. */ {R_REALISTIC, R_TRUE, R_REALISTIC}
};

/* XOR truth table:
 *     F  T  R
 * F   F  T  R
 * T   T  F  R
 * R   R  R  R
 */
static const realistic_t XOR_TABLE[3][3] = {
    /* FALSE */ {R_FALSE, R_TRUE, R_REALISTIC},
    /* TRUE  */ {R_TRUE,  R_FALSE, R_REALISTIC},
    /* REAL. */ {R_REALISTIC, R_REALISTIC, R_REALISTIC}
};

/* IMPLIES truth table:
 *     F  T  R
 * F   T  T  T
 * T   F  T  R
 * R   R  R  R
 */
static const realistic_t IMPLIES_TABLE[3][3] = {
    /* FALSE */ {R_TRUE, R_TRUE, R_TRUE},
    /* TRUE  */ {R_FALSE, R_TRUE, R_REALISTIC},
    /* REAL. */ {R_REALISTIC, R_REALISTIC, R_REALISTIC}
};

/* EQUIV truth table:
 *     F  T  R
 * F   T  F  R
 * T   F  T  R
 * R   R  R  R
 */
static const realistic_t EQUIV_TABLE[3][3] = {
    /* FALSE */ {R_TRUE, R_FALSE, R_REALISTIC},
    /* TRUE  */ {R_FALSE, R_TRUE, R_REALISTIC},
    /* REAL. */ {R_REALISTIC, R_REALISTIC, R_REALISTIC}
};

/** Logical Operations */

realistic_t realistic_not(realistic_t a)
{
    if (a < 0 || a > 2) return R_REALISTIC;
    return NOT_TABLE[a];
}

realistic_t realistic_and(realistic_t a, realistic_t b)
{
    if (a < 0 || a > 2 || b < 0 || b > 2) return R_REALISTIC;
    return AND_TABLE[a][b];
}

realistic_t realistic_or(realistic_t a, realistic_t b)
{
    if (a < 0 || a > 2 || b < 0 || b > 2) return R_REALISTIC;
    return OR_TABLE[a][b];
}

realistic_t realistic_xor(realistic_t a, realistic_t b)
{
    if (a < 0 || a > 2 || b < 0 || b > 2) return R_REALISTIC;
    return XOR_TABLE[a][b];
}

realistic_t realistic_implies(realistic_t a, realistic_t b)
{
    if (a < 0 || a > 2 || b < 0 || b > 2) return R_REALISTIC;
    return IMPLIES_TABLE[a][b];
}

realistic_t realistic_equiv(realistic_t a, realistic_t b)
{
    if (a < 0 || a > 2 || b < 0 || b > 2) return R_REALISTIC;
    return EQUIV_TABLE[a][b];
}

/** Utility Functions */

int realistic_is_true(realistic_t a)
{
    return (a == R_TRUE);
}

int realistic_is_false(realistic_t a)
{
    return (a == R_FALSE);
}

int realistic_is_realistic(realistic_t a)
{
    return (a == R_REALISTIC);
}

int realistic_is_known(realistic_t a)
{
    return (a == R_TRUE || a == R_FALSE);
}

const char* realistic_to_string(realistic_t a)
{
    switch (a) {
        case R_FALSE:     return "FALSE";
        case R_TRUE:      return "TRUE";
        case R_REALISTIC: return "REALISTIC";
        default:          return "UNKNOWN";
    }
}

realistic_t realistic_from_int(int value)
{
    switch (value) {
        case 0:  return R_FALSE;
        case 1:  return R_TRUE;
        case 2:  return R_REALISTIC;
        default: return R_REALISTIC;  /* Unknown → Uncertain */
    }
}

/** Advanced Operations */

realistic_t realistic_resolve(realistic_t a, unsigned int probability_percent)
{
    /* If already known, return as-is */
    if (a != R_REALISTIC) {
        return a;
    }
    
    /* Simple deterministic resolution based on probability
     * In a real implementation, this could use hardware RNG
     * For now, use a simple threshold */
    
    /* Use a pseudo-random approach based on current state
     * This is deterministic but varies based on input */
    unsigned int threshold = 50;
    
    if (probability_percent > threshold) {
        return R_TRUE;
    } else if (probability_percent < threshold) {
        return R_FALSE;
    } else {
        /* Exactly 50% - remain uncertain */
        return R_REALISTIC;
    }
}

unsigned int realistic_probability(realistic_t a)
{
    switch (a) {
        case R_FALSE:     return 0;    /* 0% certain it's true */
        case R_TRUE:      return 100;  /* 100% certain it's true */
        case R_REALISTIC: return 50;   /* 50% uncertain */
        default:          return 50;   /* Unknown → uncertain */
    }
}
