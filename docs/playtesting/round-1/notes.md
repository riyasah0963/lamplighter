# Playtest round 1

**Date:** 5 November 2026 · **Build:** commit `e040985` (3 districts, Creepers and Moths, oil, lamps, clock, hearts, best times) · **Where:** online

Testers are labelled T1, T2, T3. No names.

## How each session runs (about 10 minutes)

1. Tell them only: "Light every street lamp before the clock runs out. WASD or arrows move, E lights a lamp, Space opens or shuts the lantern."
2. Let them play **without help** for the first run. Watch and write down what happens.
3. Let them play 2 more runs.
4. They answer the questionnaire.

## Questionnaire

Rate 1 (strongly disagree) to 5 (strongly agree):

1. I understood what I had to do.
2. The controls were easy to use.
3. Running low on oil felt fair.
4. The clock gave me enough time.
5. The Creepers felt fair.
6. The game felt tense.
7. I wanted to play again.

Open questions:

8. What confused you?
9. What was the best part?
10. What was the worst part?
11. What one thing would you change?

About you: how often do you play video games? (never / sometimes / weekly / daily)

## Results

| Question | T1 | T2 | T3 |
| --- | --- | --- | --- |
| 1 Understood the goal | 5 | 5 | 4 |
| 2 Controls | 4 | 5 | 3 |
| 3 Oil fair | 3 | 4 | 2 |
| 4 Clock long enough | 3 | 4 | 2 |
| 5 Creepers fair | 3 | 4 | 2 |
| 6 Tense | 5 | 4 | 5 |
| 7 Play again | 5 | 5 | 4 |
| How often they play games | Daily | Weekly | Sometimes |

**Open answers**

- **T1**
  - Confused: "I was not sure whether I needed to keep the lantern open all the time or only when Creepers were nearby."
  - Best: "The Creepers running away when the lantern was open. It made the game exciting."
  - Worst: "Running low on oil while trying to light several lamps."
  - Change: "Make it clearer when I am close enough to light a lamp."
- **T2**
  - Confused: "I did not immediately understand why the Moths were following me."
  - Best: "Planning a route and trying to light all the lamps before the clock ran out."
  - Worst: "Having to backtrack for oil cans while the clock was running."
  - Change: "Add a small indicator showing how many lamps are left."
- **T3**
  - Confused: "I struggled to tell whether the lantern was open or shut."
  - Best: "The dark streets and the tension of being chased by Creepers."
  - Worst: "Losing hearts when I was caught in the dark."
  - Change: "Make the lantern status more visible and give players more time to learn the mechanics."

## What I saw

Run times were not recorded.

| Tester | Run | Win or lose (how) | Where they got stuck, what they said |
| --- | --- | --- | --- |
| T1 | 1 | Lost: ran out of hearts | Kept the lantern open frequently and attracted Moths. Was unsure how to manage oil. |
| T1 | 2 | Won: lit all lamps | Started using lit lamps to refill oil and improved route planning. |
| T1 | 3 | Won: lit all lamps | Played more confidently and used the lantern strategically. |
| T2 | 1 | Lost: clock ran out | Spent too much time exploring and backtracking for oil cans. |
| T2 | 2 | Won: lit all lamps | Planned a route and used the lantern more strategically. |
| T2 | 3 | Won: lit all lamps | Completed the district more efficiently and enjoyed improving the best time. |
| T3 | 1 | Lost: ran out of hearts | Struggled with movement and lantern controls. Got caught by Creepers several times. |
| T3 | 2 | Lost: clock ran out | Understood the controls better but spent too much time avoiding enemies. |
| T3 | 3 | Won: lit all lamps | Learned to shut the lantern when Moths approached and completed the district. |

## Findings

Each finding is a GitHub Issue, closed by the commit that fixes it.

1. **The lantern's state and low oil are hard to see.** T3 could not tell whether the lantern was open or shut; T1 was unsure how to manage oil. [Issue #1](https://github.com/riyasah0963/lamplighter/issues/1)
2. **It is not clear when a lamp is close enough to light, or which lamps are left.** T1 asked for this; T2 asked for an indicator of lamps left. [Issue #2](https://github.com/riyasah0963/lamplighter/issues/2)
3. **The clock and oil are tight for new players.** Average ratings: oil fair 3.0, clock long enough 3.0, Creepers fair 3.0 (T3 gave 2 for each); 2 of 9 runs ended with the clock running out; T3 asked for more time. [Issue #3](https://github.com/riyasah0963/lamplighter/issues/3)
