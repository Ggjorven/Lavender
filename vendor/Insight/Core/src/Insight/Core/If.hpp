#pragma once

#define INSIGHT_CONC_HELPER(a, b) a##_##b
#define INSIGHT_CONC(a, b) INSIGHT_CONC_HELPER(a, b)

#define INSIGHT_IF(c, t, e) INSIGHT_CONC(INSIGHT_IF, c)(t, e)
#define INSIGHT_IF_1(t, e) e
#define INSIGHT_IF_0(t, e) t

// Example usage
/*
```cpp
#define OPTION1 1
#define OPTION0 0


#define TEST(option) INSIGHT_IF(option, int option0 = 0, int option1 = 0)


TEST(OPTION0);
```
*/