"""proto-Nucleic Acid Builder
proto-Nucleic Acid Builder

Main file
"""

from __future__ import division, absolute_import, print_function

import os
import yaml
import itertools
from io import StringIO
import multiprocessing as mp
import warnings
warnings.filterwarnings("ignore", category=UserWarning)
import datetime
import numpy as np

from pnab import __path__
from pnab import bind
from pnab.driver import options


class pNAB(object):
    """ pNAN run class

    A class that contains methods to create a pNAB run. It makes input,
    runs it, and parses results.
    """

    def __init__(self, file_path):
        """The constructor"""

        if not os.path.isfile(file_path):
            file_path = os.path.join(__path__[0], 'data', file_path)
        self.options = yaml.load(open(file_path, 'r'), yaml.FullLoader)
        options._validate_all_options(self.options)
        self._input_file = file_path

        # Add library of bases
        data_dir = os.path.join(__path__[0], 'data')
        bases_lib = yaml.load(open(os.path.join(data_dir, 'bases_library.yaml'), 'r'), yaml.FullLoader)
        for b in bases_lib.values():
            b['file_path'] = os.path.join(data_dir, b['file_path'])
        if self.options['RuntimeParameters'].pop('pair_A_U', None):
            bases_lib['Base A']['pair_name'] = 'U'
        self.options.update(bases_lib)


    def _run(self, config):
        """ function to run one helical configuration."""
        config, prefix = config[0], config[1]

        runtime_parameters = bind.RuntimeParameters()
        [runtime_parameters.__setattr__(k, val) for k, val in self.options['RuntimeParameters'].items()]

        backbone = bind.Backbone()
        [backbone.__setattr__(k, val) for k, val in self.options['Backbone'].items()]

        py_bases = [self.options[i] for i in self.options if 'Base' in i]
        bases = [bind.Base() for i in range(len(py_bases))]
        for i, b in enumerate(py_bases):
            [bases[i].__setattr__(k, val) for k, val in b.items()]

        helical_parameters = bind.HelicalParameters()
        [helical_parameters.__setattr__(k, val) for k, val in zip(self.options['HelicalParameters'], config)]
        result = bind.run(runtime_parameters, backbone, bases, helical_parameters, prefix)
        result = np.genfromtxt(StringIO(result), delimiter=',')

        if result.size == 0:
            result = None
        elif result.ndim == 1:
            result = result.reshape(1, len(result))

        header = ''.join(['%s=%.2f, ' %(k, val) for k, val in zip(self.options['HelicalParameters'], config)])
        header = header.strip(', ')

        return [prefix, header, result] 


    def _single_result(self, results):
        """ Extract results for each helical configuration as it finishe."""

        with open('prefix.yaml', 'ab') as f:
            f.write(str.encode(yaml.dump({results[0]:results[1]})))

        if results[2] is None:
            return

        header = results[1] + '\n'
        header += ('Prefix, Conformer Index, Distance (Angstroms), Bond Energy (kcal/mol/bond), Angle Energy (kcal/mol/angle), ' +
                   'Torsion Energy (kcal/mol/nucleotide), Van der Waals Energy (kcal/mol/nucleotide), ' + 
                   'Total Energy (kcal/mol/nucleotide), Fixed Torsions Energy (kcal/mol/nucleotide), RMSD (Angstrom)')

        with open('results.csv', 'ab') as f:
            np.savetxt(f, results[2], delimiter=',', header=header)


    def run(self):
        """ Fuction to prepare helical configurations and run them in parallel."""

        # Extract configurations
        config = itertools.product(*[np.random.uniform(val[0], val[1], val[2])
                                       for val in self.options['HelicalParameters'].values()])
        num_config = np.prod([val[2] for val in self.options['HelicalParameters'].values()])
        prefix = (str(i) for i in range(1, num_config + 1))

        # Catch interruption
        import signal
        def init_worker():
            signal.signal(signal.SIGINT, signal.SIG_IGN)

        pool = mp.Pool(mp.cpu_count(), init_worker)

        for f in ['results.csv', 'prefix.yaml', 'summary.csv']:
            file_path = f
            while True:
                if os.path.isfile(file_path):
                    file_path = '_' + file_path
                else:
                    if os.path.isfile(f):
                        os.rename(f, file_path)
                    break

        time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        with open('results.csv', 'w') as f: f.write('# ' + time + '\n')
        with open('prefix.yaml', 'w') as f: f.write('# ' + time + '\n')

        try:
            for results in pool.imap(self._run, zip(config, prefix)):
                self._single_result(results)
        except KeyboardInterrupt:
            print("Caught interruption; stopping ...")
            pool.terminate()

        pool.close()


        #Extract the results from the run and report it to the user
        self.prefix = yaml.load(open('prefix.yaml'), yaml.FullLoader)

        self.header = ('Prefix, Conformer Index, Distance (Angstroms), Bond Energy (kcal/mol/bond), Angle Energy (kcal/mol/angle), ' +
                       'Torsion Energy (kcal/mol/nucleotide), Van der Waals Energy (kcal/mol/nucleotide), ' + 
                       'Total Energy (kcal/mol/nucleotide), Fixed Torsions Energy (kcal/mol/nucleotide), RMSD (Angstrom)')

        time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        self.results = np.loadtxt('results.csv', delimiter=',')
        if self.results.size == 0:
            print("No candidate found")
            return

        elif self.results.ndim == 1:
            self.results = self.results.reshape(1, len(self.results))

        self.results = self.results[self.results[:, 7].argsort()]
        summary = self.results[:10]

        np.savetxt('summary.csv', summary, delimiter=',', header=time + '\n' + self.header)
