using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EnvDTE;

namespace CoatiSoftware.CoatiPlugin
{
    class SolutionUtility
    {
        public static String GetSolutionPath(DTE dte)
        {
            EnvDTE.Solution solution = dte.Solution;

            return solution.FullName;
        }

        public static List<String> GetSolutionProjects(DTE dte)
        {
            List<String> projectNames = new List<String>();

            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                projectNames.Add(project.Name);
            }

            return projectNames;
        }

        public static List<String> GetSolutionProjectsFullNames(DTE dte)
        {
            List<String> projectNames = new List<String>();

            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                projectNames.Add(project.FullName);
            }

            return projectNames;
        }

        public static List<List<String>> GetSolutionProjectItems(DTE dte)
        {
            List<List<String>> projectItems = new List<List<String>>();

            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                EnvDTE.ProjectItems items = project.ProjectItems;                

                List<String> pItems = new List<String>();

                foreach(EnvDTE.ProjectItem item in items)
                {
                    for (short i = 0; i < item.FileCount; i++)
                    {
                        pItems.Add(item.get_FileNames(i));
                    }
                }

                projectItems.Add(pItems);
            }

            return projectItems;
        }

        public static List<String> GetSolutionLanguages(DTE dte)
        {
            List<String> languages = new List<String>();

            EnvDTE.Solution solution = dte.Solution;
            EnvDTE.Projects projects = solution.Projects;

            foreach (EnvDTE.Project project in projects)
            {
                if (project.CodeModel != null)
                {
                    string language = project.CodeModel.Language;
                    languages.Add(language);
                }
            }

            languages = languages.Distinct().ToList();

            return languages;
        }

        public static bool GetSolutionIsSaved(DTE dte)
        {
            EnvDTE.Solution solution = dte.Solution;

            return solution.Saved;
        }
    }
}
