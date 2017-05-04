#ifndef TRAINBOOM_ROUTE_HPP
#define TRAINBOOM_ROUTE_HPP

#include "util.hpp"
#include "Segment.hpp"
#include "Information.hpp"
#include <iostream>
#include <sstream>

namespace TrainBoom {

typedef util::IntervalManip<
    Segment,
    TicketDelta,
    Segment::Modifier,
    Segment::MergerT,
    Segment::MergerM
> SegmentsInvervalManip;

class Information;

class Route {
private:
    std::string name;

    unsigned n;

    util::stupid_array<Information> informations;

    util::stupid_array<Segment> segments;

    util::stupid_ptr<SegmentsInvervalManip> segmentsIntervalManip;
    util::map<Id, unsigned> stationsMap;

    Id id;

    util::pair<unsigned, unsigned> getInterval(Id startStation, Id endStation) {
        const auto& iterStart = stationsMap.find(startStation),
            iterEnd = stationsMap.find(endStation);
        if (iterStart == stationsMap.end() ||
            iterEnd == stationsMap.end()) {
                throw station_not_found();
            }
        unsigned start = iterStart->second, end = iterEnd->second;
        if (end <= start) {
            throw interval_invalid();
        }

        return util::make_pair(start, end - 1); // Attenion here !!
    }

public:
    class station_number_too_small : public exception {
    public:
    	station_number_too_small() : exception(
    		"station_number_too_small",
    		"Your station number is too small!!!") {}
    };

    class station_number_not_consistent : public exception {
    public:
    	station_number_not_consistent() : exception(
    		"station_number_not_consistent",
    		"Your station number is not consistent among all information!!!") {}
    };

    class station_not_found : public exception {
    public:
    	station_not_found() : exception(
    		"station_not_found",
    		"Your station is not found!!!") {}
    };

    class index_out_of_range : public exception {
    public:
    	index_out_of_range() : exception(
    		"index_out_of_range",
    		"Your index is out of range!!!") {}
    };

    class interval_invalid : public exception {
    public:
    	interval_invalid() : exception(
    		"interval_invalid",
    		"Your requested interval is not valid!!!") {}
    };

    explicit Route(): id("Route") {}

    /*Route(Id id, unsigned n,
        const util::stupid_array<Id>& stations,
        const util::stupid_array<unsigned>& distance,
        const util::stupid_array<util::Datetime::Datetime>& arriveTime,
        const util::stupid_array<util::Datetime::Datetime>& leaveTime,
        const util::stupid_array<Segment>& segments
    ): id(id), n(n), stations(stations), distance(distance),
        arriveTime(arriveTime), leaveTime(leaveTime), segments(segments) {
            if (n < 2) {
                throw station_number_too_small();
            }
            if (stations.size() != n || distance.size() != n - 1 ||
                arriveTime.size() != n - 1 || leaveTime.size() != n - 1 ||
                segments.size() != n - 1) {
                    throw station_number_not_consistent();
                }

            for (unsigned i = 0; i < n; ++ i)
                stationsMap[stations[i]] = i;

            segmentsIntervalManip = new SegmentsInvervalManip(
                segments, n - 1
            );
        }*/

    Route(std::string name, unsigned n,
        const util::stupid_array<Information>& informations,
        const util::stupid_array<Segment>& segments
    ): name(name), n(n), informations(informations), segments(segments), id("Route") {
            if (n < 2) {
                throw station_number_too_small();
            }
            if (informations.size() != n || segments.size() != n - 1) {
                    throw station_number_not_consistent();
                }
            for (unsigned i = 0; i < n; ++ i)
                stationsMap[informations[i].getStationId()] = i;
            segmentsIntervalManip = new SegmentsInvervalManip(
                segments, n - 1
            );
        }

    /*void rebuild(unsigned _n,
        const util::stupid_array<Id>& _stations,
        const util::stupid_array<unsigned>& _distance,
        const util::stupid_array<util::Datetime::Datetime>& _arriveTime,
        const util::stupid_array<util::Datetime::Datetime>& _leaveTime,
        const util::stupid_array<Segment>& _segments) {
            if (_n < 2) {
                throw station_number_too_small();
            }
            if (_stations.size() != n || _distance.size() != n - 1 ||
                _arriveTime.size() != n - 1 || _leaveTime.size() != n - 1 ||
                _segments.size() != n - 1) {
                    throw station_number_not_consistent();
                }

            n = _n;
            stations = _stations;
            distance = _distance;
            arriveTime = _arriveTime;
            leaveTime = _leaveTime;
            segments = _segments;

            stationsMap.clear();
            for (unsigned i = 0; i < n; ++ i)
                stationsMap[stations[i]] = i;

            segmentsIntervalManip = new SegmentsInvervalManip(
                segments, n - 1
            );
        }*/

    Id getId() const {
        return id;
    }

    void modifyTickets(Id startStation, Id endStation, const TicketDelta& deltas) {
        auto interval = getInterval(startStation, endStation);
        segmentsIntervalManip->modify(interval.first, interval.second, deltas);
    }

    Segment queryTickets(Id startStation, Id endStation) {
        auto interval = getInterval(startStation, endStation);
        return segmentsIntervalManip->query(interval.first, interval.second);
    }

    Information& information(unsigned pos) {
        if (pos >= n) {
            throw index_out_of_range();
        }
        return informations[pos];
    }

    Information information(unsigned pos) const {
        if (pos >= n) {
            throw index_out_of_range();
        }
        return informations[pos];
    }

    util::Datetime::Datetime getRunningDay() const noexcept {
        return informations[0].getLeaveTime().clearTime();
    }

    void display() {
        segmentsIntervalManip->forceApply();
        std::cout << "\n----\n" << std::endl;
        std::cout << "id: " << id << std::endl;
        std::cout << "nStation: " << n << std::endl;
        for (unsigned i = 0; i < n; ++ i) {
            std::cout << "\nStation #" << i << ": " << std::endl;
            informations[i].display();
            if (i < n - 1) {
                std::cout << "\tTicket information: \n" << std::endl;
                segments[i].display();
                // std::cout << "#####################" << std::endl;
            }
        }
        std::cout << "\n---\n" << std::endl;
    }

    util::Json toJson() {
        segmentsIntervalManip->forceApply();

        util::Json json("route", id);

        json["id"] = id;
        json["name"] = name;
        json["n"] = n;
        json["informations"].SetArray();
        json["segments"].SetArray();
        // json["stationsMap"].SetObject();

        for (unsigned int i = 0; i < n; ++ i) {
            json["informations"].PushBack(informations[i].toJson());
            // std::cout << "!!asdasd!" << std::endl;
            if (i + 1 < n)
                json["segments"].PushBack(segments[i].toJson());
            // std::cout << "!!asdasd!" << i << std::endl;
        }

        /* for (const auto& item: stationsMap) {
            json["stationsMap"][item.first] = item.second;
        } */

        return json;
    }

    std::string toString() {
        segmentsIntervalManip->forceApply();
        std::stringstream ss;
        ss << "name " << name << '\n';
        ss << "n " << n << '\n';
        ss << "informations " << n << '\n';
        for (unsigned i = 0; i < n; ++ i)
            ss << "information " << informations[i].getId() << '\n';
        ss << "segments " << n - 1 << '\n';
        for (unsigned i = 0; i < n - 1; ++ i)
            ss << "segment " << segments[i].getId() << '\n';
        return ss.str();
    }
};

}   // TrainBoom

#endif
