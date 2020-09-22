/*
   author: rescr1pt@.ya.ru
*/

#ifndef SLEEPMYPC_DASHBOARD_DETAILS
#define SLEEPMYPC_DASHBOARD_DETAILS

enum
{
    DASHOBARD_DETAIL_COUNT = 4
};

struct ActiveDetails
{
    ActiveDetails()
    {
        allDetails_ = 0;

        for (unsigned char i = 0; i < DASHOBARD_DETAIL_COUNT; ++i) {
            allDetails_ |= (1 << i);
        }

        value_ = allDetails_;
    }

    inline bool hasDetail(unsigned char detailIndex) const { return ((value_) & (1 << detailIndex)) != 0; }
    inline void enableDetail(unsigned char detailIndex) { value_ |= (1 << detailIndex); }
    inline void disableDetail(unsigned char detailIndex) { value_ = ((value_) & ~(1 << detailIndex)); }

    inline bool hasAllDetails() const { return value_ == allDetails_; }
    inline bool hasDetails() const { return value_ > 0; }

    unsigned char value_;

private:
    unsigned char allDetails_;
};

#endif