





def gcd(*numbers):
    """Return the greatest common divisor of the given integers"""
    from fractions import gcd
    return reduce(gcd, numbers)


def lcm(*numbers):
    """Return lowest common multiple."""    
    def lcm(a, b):
        return (a * b) // gcd(a, b)

    return reduce(lcm, numbers, 1)


def Schedule( channels ):
    """
    """
    utilisation     = 0
    periodSet       = set()
    for channel in channels:
        periodSet.add(channel['Period'])
        utilisation     = utilisation + (float(channel['Duration']) / channel['Period'])

    totalPeriod     = lcm(*periodSet)

    schedule    = []
    for iteration in range(0,totalPeriod):
        activeSet   = set()
        for channel in channels:
            if iteration%channel['Period'] == 0:
                activeSet.add(channel['ID'])
        schedule.append(activeSet)

    return {'totalPeriod':totalPeriod, 'Utilisation':utilisation, 'Periods':periodSet, 'Schedule':schedule} 


if __name__ == '__main__':
    """
    """
    print('Scheduler')

    channels    = [
                    {'ID':0, 'Duration':1, 'Period':8} ,
                    {'ID':1, 'Duration':1, 'Period':16} ,
                    {'ID':2, 'Duration':2, 'Period':8} ,
                    {'ID':3, 'Duration':1, 'Period':16} ,
                    {'ID':4, 'Duration':1, 'Period':32} ,
                    {'ID':5, 'Duration':1, 'Period':3} ,
                    {'ID':6, 'Duration':1, 'Period':16} ,
                    {'ID':7, 'Duration':1, 'Period':16} ,
                  ]

    result    = Schedule(channels)
    print(result)
    schedule    = result['Schedule']

    if result['Utilisation'] > 1.0:
        print('!Not schedulable!')
    else:
        i   = 0
        for iteration in schedule:
            print('%d) %s'%(i, iteration) )
            i = i + 1


