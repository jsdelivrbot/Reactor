





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
            print('@%d -> %d = %d'%(timeslot,closestTimeslot, closestTimeslot-timeslot))
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
    print('Scheduler')

    channels    = [
                    {'ID':0, 'Duration':1, 'Period':8,   'AcceptableError':0.1} ,
                    {'ID':1, 'Duration':1, 'Period':16,  'AcceptableError':0.1} ,
                    {'ID':2, 'Duration':2, 'Period':8,   'AcceptableError':0.1} ,
                    {'ID':3, 'Duration':1, 'Period':16,  'AcceptableError':0.1} ,
                    {'ID':4, 'Duration':1, 'Period':32,  'AcceptableError':0.1} ,
                    {'ID':5, 'Duration':1, 'Period':3,   'AcceptableError':0.1} ,
                    {'ID':6, 'Duration':1, 'Period':16,  'AcceptableError':0.1} ,
                    {'ID':7, 'Duration':1, 'Period':16,  'AcceptableError':0.1} ,
                  ]

    result    = Schedule(channels)
    #print(result)
    schedule    = result['Schedule']

    if result['Utilisation'] > 1.0:
        print('!Not schedulable!')
    else:
        i   = 0
        for iteration in schedule:
            print('%d) %s'%(i, iteration) )
            i = i + 1


