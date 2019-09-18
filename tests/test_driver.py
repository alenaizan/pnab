from __future__ import division, absolute_import, print_function

import os
import numpy as np

def test_options():
    """
    test parity between the C++ attributes of classes and the python options
    """
    from pnab import bind
    from pnab.driver.options import _options_dict

    components = ['Backbone', 'RuntimeParameters', 'HelicalParameters']
    assert all([i in _options_dict for i in components])


    assert all([i in _options_dict['Backbone'] for i in bind.Backbone.__dict__
                if not i.startswith('__')])
    assert all([i in _options_dict['RuntimeParameters'] for i in
                bind.RuntimeParameters.__dict__ if not i.startswith('__')])
    assert all([i in _options_dict['HelicalParameters'] for i in
                bind.HelicalParameters.__dict__ if not i.startswith('__')])


def test_run():
    """
    test running with provided option file
    """
    import pnab

    os.chdir(os.path.dirname(os.path.realpath(__file__)))

    run = pnab.pNAB('RNA.yaml')
    run.run()

    ref_output = np.genfromtxt(os.path.join('files','RNA.csv'), delimiter=',')
    assert np.allclose(run.results, ref_output)


def test_run_range():
    """
    test running with provided option file
    """
    import pnab

    os.chdir(os.path.dirname(os.path.realpath(__file__)))

    run = pnab.pNAB('RNA2.yaml')
    
    run.run()

    ref_output = np.genfromtxt(os.path.join('files','RNA2.csv'), delimiter=',')

    print(run.options)
    print(run.prefix)
    print(run.results)

    assert len(run.prefix) == 15
    assert np.allclose(run.results, ref_output)


def test_duplex():
    """
    test generating duplex with provided option file
    """
    import pnab

    os.chdir(os.path.dirname(os.path.realpath(__file__)))

    run = pnab.pNAB('DNA.yaml')
    run.run()

    ref_output = np.genfromtxt(os.path.join('files','DNA.csv'), delimiter=',')
    assert np.allclose(run.results, ref_output)


def test_hexad():
    """
    test generating hexad with provided option file
    """
    import pnab

    os.chdir(os.path.dirname(os.path.realpath(__file__)))

    run = pnab.pNAB('Hexad.yaml')
    run.run()

    ref_output = np.genfromtxt(os.path.join('files','Hexad.csv'), delimiter=',')
    assert np.allclose(run.results, ref_output)
