import os, json, datetime
from ContinuousRegistration.Source.make_registration_scripts import parser
from ContinuousRegistration.Source.util import logging, load_submissions
from ContinuousRegistration.Source.datasets import load_datasets

def write_json(output_file_name, results):
    output_directory = os.path.dirname(output_file_name)
    if not os.path.exists(output_directory):
        os.mkdir(output_directory)

    with open(output_file_name, "w") as f:
        f.write(json.dumps(results, indent=4))

def run(parameters):
    submissions = load_submissions(parameters)
    datasets = load_datasets(parameters)

    results = {}
    for team_name, blueprint_file_names in submissions.items():
        for blueprint_file_name in blueprint_file_names:
            if parameters.blueprint_file_name is None:
                # User requested evaluation for these blueprints only
                if parameters.blueprint_file_name is not None and \
                   os.path.basename(blueprint_file_name) in parameters.blueprint_file_name:
                    continue

            if not team_name in results:
                results[team_name] = {}

            blueprint_name, blueprint_ext = os.path.splitext(os.path.basename(blueprint_file_name))
            if not blueprint_name in results[team_name]:
                results[team_name][blueprint_name] = {}

            logging.info('Loading blueprint %s/%s.' % (team_name, os.path.basename(blueprint_name)))
            blueprint = json.load(open(blueprint_file_name))
            for dataset_name in blueprint['Datasets']:
                if not dataset_name in datasets:
                    logging.info('Blueprint "%s" can also be used for dataset "%s". '
                                 'Supply data directory to evaluate blueprint on this dataset '
                                 '(registrations must be run first).',
                                  blueprint_file_name, dataset_name)
                    continue

                dataset = datasets[dataset_name]
                results[team_name][blueprint_name][dataset_name] = []

                logging.info('Evaluating registration scripts for %s blueprint and %s dataset.' % (
                blueprint_name, dataset_name))
                for file_names in dataset.generator():
                    output_directory = os.path.join(parameters.output_directory, team_name, blueprint_name)

                    logging.info('Evaluating image pair %s.', file_names['image_file_names'])
                    try:
                        results[team_name][blueprint_name][dataset.name].append(dataset.evaluate(
                            parameters.superelastix, file_names, output_directory))

                        if parameters.make_images:
                            dataset.make_images(parameters.superelastix, file_names, output_directory)

                        if parameters.make_labels:
                            dataset.make_labels(parameters.superelastix, file_names, output_directory)

                        if parameters.make_checkerboards:
                            dataset.make_checkerboards(parameters.superelastix, file_names, output_directory)

                        if parameters.make_image_checkerboards:
                            dataset.make_image_checkerboards(parameters.superelastix, file_names, output_directory)

                        if parameters.make_label_checkerboards and dataset.name in ["CUMC12", "ISBR18", "LPBA40", "MGH10"]:
                            dataset.make_label_checkerboards(parameters.superelastix, file_names, output_directory)
                    except Exception as e:
                        logging.error('Error during evaluation of %s\'s blueprint %s and dataset %s: %s'
                                      % (team_name, blueprint_name, dataset.name, str(e)))


    write_json(os.path.join(parameters.output_directory,
                            'results.json'), results)

if __name__ == '__main__':
    parser.add_argument('--make-images', '-mi', type=bool, default=False, help="Warp moving images.")
    parser.add_argument('--make-labels', '-ml', type=bool, default=False, help="Warp moving labels.")
    parser.add_argument('--make-checkerboards', '-mc', type=bool, default=False, help="Warp checkerboard pattern.")
    parser.add_argument('--make-image-checkerboards', '-mic', type=bool, default=False,
                        help="Warp moving images and make checkerboard with fixed and warped moving image.")
    parser.add_argument('--make-label-checkerboards', '-mlc', type=bool, default=False,
                        help="Warp moving labels and make checkerboard with fixed and warped moving label.")
    run(parser.parse_args())
