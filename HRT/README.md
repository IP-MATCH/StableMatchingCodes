# Naming

The following table relates these folders to the names used in the paper


| Folder name             | Algorithm index in paper
|-------------------------|--------------------------|
| 1_NOBIN_1STA_NOMERGED   | N1                       |
| 2_YESBIN_1STA_NOMERGED  | N2                       |
| 3_NOBIN_2STA_NOMERGED   | N3                       |
| 4_YESBIN_2STA_NOMERGED  | N4                       |
| 5_NOBIN_2STA_YESMERGED  | N5                       |
| 6_YESBIN_2STA_YESMERGED | N6                       |
| 7_NOBIN_3STA_NOMERGED   | N7                       |
| 8_YESBIN_3STA_NOMERGED  | N8                       |
| 9_NOBIN_3STA_YESMERGED  | N9                       |
| A_YESBIN_3STA_YESMERGED | N10                      |
| B_NOBIN_4STA_NOMERGED   | N11                      |
| C_YESBIN_4STA_NOMERGED  | N12                      |


# Improved preprocessing codes

Two of these folders (corresponding to N4 and N8) have been used for improved
preprocessing. The available preprocessing modes for HRT are as follows (for usage instructions, see [the earlier README.md](../README.md):

| Mode (as program option) | Mode (from paper) | Description                           |
|--------------------------|-------------------|---------------------------------------|
| 0                        | P1*               | Hospitals offer / Residents apply     |
| 1                        | P2                | Skip 5                                |
| 2                        |                   | Find best                             |
| 3                        | P3                | Skip 15                               |
| 4                        | P4                | Skip 50                               |
| 5                        |                   | Best in group                         |
| 6                        | P5                | Graph-based (family first)            |
| 7                        |                   | Mode 1, then Mode 6                   |
| 8                        |                   | Graph-based (children first)          |
| 9                        | P6                | Extended graph-based (family first)   |
| 10                       |                   | Extended graph-based (children first) |
| 11                       |                   | Mode 1, then Mode 8                   |
| 12                       | P0                | No preprocessing                      |

