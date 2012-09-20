ecalEnergyTask = dict(
    isPhysicsRun = True,
    threshS9 = 0.125,
    MEs = dict(
        HitMap = dict(path = '%(subdet)s/%(prefix)sOccupancyTask/%(prefix)sOT rec hit energy %(sm)s', otype = 'SM', btype = 'Crystal', kind = 'TProfile2D', zaxis = {'title': 'energy (GeV)'}),
        HitMapAll = dict(path = '%(subdet)s/%(prefix)sSummaryClient/%(prefix)sOT%(suffix)s energy summary', otype = 'Ecal3P', btype = 'SuperCrystal', kind = 'TProfile2D', zaxis = {'title': 'energy (GeV)'}),
        Hit = dict(path = '%(subdet)s/%(prefix)sOccupancyTask/%(prefix)sOT energy spectrum %(sm)s', otype = 'SM', btype = 'User', kind = 'TH1F', xaxis = {'nbins': 100, 'low': 0., 'high': 20., 'title': 'energy (GeV)'}),
        HitAll = dict(path = '%(subdet)s/%(prefix)sOccupancyTask/%(prefix)sOT rec hit spectrum%(suffix)s', otype = 'Ecal3P', btype = 'User', kind = 'TH1F', xaxis = {'nbins': 100, 'low': 0., 'high': 20., 'title': 'energy (GeV)'}),
#        MiniCluster = dict(path = 'Energy/Spectrum/3x3/EnergyTask 3x3', otype = 'SM', btype = 'User', kind = 'TH1F', xaxis = {'nbins': 100, 'low': 0., 'high': 20., 'title': 'energy (GeV)'})
    )
)

    