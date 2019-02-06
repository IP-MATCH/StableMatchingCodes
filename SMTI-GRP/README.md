# Naming

The following table relates these folders to the names used in the paper

| Folder name | Algorithm index in paper
|-------------|-------------------------|
|1_NOBIN_1STA_NOMERGED | M1 |
|2_YESBIN_1STA_NOMERGED | M2 |
|3_NOBIN_1STA_YESMERGED | M3 |
|4_YESBIN_1STA_YESMERGED | M4 |
|7_NOBIN_2STA_YESMERGED | M5 |
|8_YESBIN_2STA_YESMERGED | M6 |

The folder 1_NOBIN_1STA_NOMERGED_NOPREPROCESS is M1 but with preprocessing disabled.
The folder 1_NOBIN_1STA_NOMERGED_NOPREPROCESS_NOPRESOLVE is M1 but with preprocessing and Gurobi pre-solving disabled.
The folder 4_YESBIN_1STA_YESMERGED_NOPREPROCESS is M4 but with preprocessing disabled.
The folder 4_YESBIN_1STA_YESMERGED_PRIORITY is M4 with priorities on branching variables.
The folder 5_NOBIN_2STA_NOMERGED uses 2 stability constraints, but no dummy
variables or merged stability constraints, and is not in the paper.
The folder 6_YESBIN_2STA_NOMERGED uses 2 stability constraints, dummy
variables, but does not merge stability constraints, and is not in the paper.
The folder 9_NOBIN_1STA_NOMERGED_GS is M1 with Gale-Shapley used to find an
incumbent solution.
The folder A_YESBIN_1STA_YESMERGED_GS is M4 with Gale-Shapley used to find an
incumbent solution.

