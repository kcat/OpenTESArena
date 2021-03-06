#include <cassert>

#include "Date.h"

#include "Month.h"
#include "Weekday.h"
#include "Year.h"

Date::Date(int day, WeekdayName weekdayName, MonthName monthName, const Year &year)
{
	// Programmer error if the day is ever out of range.
	assert(day >= 1);
	assert(day <= 30);

	this->day = day;

	this->weekday = std::unique_ptr<Weekday>(new Weekday(weekdayName));
	this->month = std::unique_ptr<Month>(new Month(monthName));
	this->year = std::unique_ptr<Year>(new Year(year));
}

Date::Date(const Date &date)
	: Date(date.getDayNumber(), date.getWeekday().getWeekdayName(), 
		date.getMonth().getMonthName(), date.getYear()) { }

Date::~Date()
{

}

int Date::getDayNumber() const
{
	return this->day;
}

const Weekday &Date::getWeekday() const
{
	return *this->weekday.get();
}

const Month &Date::getMonth() const
{
	return *this->month.get();
}

const Year &Date::getYear() const
{
	return *this->year.get();
}

std::string Date::getOrdinalDay() const
{
	// Programmer error if the day is ever out of range.
	assert(this->day >= 1);
	assert(this->day <= 30);

	int dayNumber = this->getDayNumber();
	int ordinalDay = dayNumber % 10;

	auto dayString = std::to_string(dayNumber);
	if (ordinalDay == 1)
	{
		dayString += "st";
	}
	else if (ordinalDay == 2)
	{
		dayString += "nd";
	}
	else if (ordinalDay == 3)
	{
		dayString += "rd";
	}
	else
	{
		dayString += "th";
	}

	return dayString;
}

void Date::incrementDay()
{
	// Programmer error if the day is ever out of range.
	assert(this->day >= 1);
	assert(this->day <= 30);

	this->day++;
	this->incrementWeekday();
	
	// No need to check for "> 31"; the assertions take care of that.
	if (this->day == 31)
	{
		this->day = 1;
		this->incrementMonth();
	}
}

void Date::incrementWeekday()
{
	this->weekday->incrementWeekday();
}

void Date::incrementMonth()
{
	bool isLastMonth = this->month->isLastMonthInYear();

	this->month->incrementMonth();

	if (isLastMonth)
	{
		this->incrementYear();
	}
}

void Date::incrementYear()
{
	this->year->incrementYear();
}
