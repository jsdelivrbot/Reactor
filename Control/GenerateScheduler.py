


import os
import sys
import importlib



def gcd(*numbers):
    """Return the greatest common divisor of the given integers"""
    from fractions import gcd
    return reduce(gcd, numbers)


def lcm(*numbers):
    """Return lowest common multiple."""    
    def lcm(a, b):
        return (a * b) // gcd(a, b)

    return reduce(lcm, numbers, 1)



def FindFirstEmptyTimeslot(timeslots):
    """
    """
    for i in range(0,len(timeslots)):
        if timeslots[i] == -1:
            return i

    return -1



def FindClosestEmptyTimeslot(timeslot, timeslots):
    """
    """
    for i in range(0,8):
        if timeslots[timeslot+i] == -1:
            return timeslot+i
        if timeslots[timeslot-i] == -1:
            return timeslot-i

    return -1


def AllocateChannelsToTimeslots(numberOfTimeSlots, channels):
    """
    """

    timeslots   = [-1]*numberOfTimeSlots

    for channel in channels:
        timeslot    = FindFirstEmptyTimeslot(timeslots)

        while timeslot < len(timeslots):

            closestTimeslot = FindClosestEmptyTimeslot(timeslot, timeslots)
            #print('@%d -> %d = %d'%(timeslot,closestTimeslot, closestTimeslot-timeslot))
            timeslots[closestTimeslot] = channel['ID']

            timeslot    = timeslot + channel['Period']


    return timeslots




def Schedule( channels ):
    """
    """
    utilisation     = 0
    periodSet       = set()
    for channel in channels:
        periodSet.add(channel['Period'])
        utilisation     = utilisation + (float(channel['Duration']) / channel['Period'])

    totalPeriod     = lcm(*periodSet)

    schedule    = AllocateChannelsToTimeslots( totalPeriod, channels )

    return {'totalPeriod':totalPeriod, 'Utilisation':utilisation, 'Periods':periodSet, 'Schedule':schedule} 


if __name__ == '__main__':
    """
    """

    #
    # Import the settings file.
    #
    settingsFile    = sys.argv[1]
    sys.path.append(os.path.dirname(settingsFile))
    moduleName,moduleExt      = os.path.splitext(os.path.basename(settingsFile))
    settings = importlib.import_module(moduleName)

    #
    #
    #
    result    = Schedule(settings.channels)
    schedule    = result['Schedule']
    text        = ''

    if result['Utilisation'] > 1.0:
        print('!Not schedulable!')
    else:
        i   = 0
        for channel in schedule:
            if channel != -1:
                text    += '        PROCESS_SCHEDULEE(%d, schedulee%d);   // %d) ch%d\n'%(channel,channel+1, i, channel)
            else:
                text    += '        WAIT_FOR_NEXT_TIMESLOT();             // %d) \n'%(i)

            i = i + 1

    #
    # Fill in the template.
    #
    template    = open(sys.argv[2]).read()
    print(template%text)


