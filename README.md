# F1-Champioship-Battle-Calculator
Calculates the chances for either of two driver to win the title. Lewis Hamilton and Max Verstappen that is for 2021.

This should comply with all the rules, including fastest laps, sprint races, half points and cancelled races. There are defines to turn off half points, cancelled races and fastest laps.

First it calculates all possible point combinations between the two drivers for every remaining (after Mexico) race. Then it finds every combination of those results. Currently that's about 3.7 trillion (but takes only 10 seconds, wow).

As of now, the chances are

Max wins 85.9695 %
Lewis wins 13.3961 %
Draw 0.634362 %

Max wins in every draw case so it is 86.6 % for Max.

There are 3 scripts with different build flags. build_basic.sh is, well, basic, build.sh optimised single core and build_omp.sh uses OpenMP to parallelise the slowest part.

Inspiration: https://old.reddit.com/r/formula1/comments/qpq7c9/i_was_bored_so_i_made_a_calculator_to_calculate/
