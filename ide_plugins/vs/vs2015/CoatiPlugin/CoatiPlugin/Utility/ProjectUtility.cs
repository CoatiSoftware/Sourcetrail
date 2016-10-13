using EnvDTE;
using System.Collections;
using System.Collections.Generic;

namespace CoatiSoftware.CoatiPlugin.Utility
{
    public class ProjectUtility
    {
        public static bool ContainsCFiles(Project project)
        {
            List<ProjectItem> projectItems = GetProjectItems(project);

            foreach (EnvDTE.ProjectItem item in projectItems)
            {
                if (item.FileCodeModel != null && item.FileCodeModel.Language == CodeModelLanguageConstants.vsCMLanguageVC)
                {
                    string extension = item.Properties.Item("Extension").Value.ToString();
                    if (extension == ".c")
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        static public List<ProjectItem> GetProjectItems(Project project)
        {
            List<ProjectItem> items = new List<ProjectItem>();

            IEnumerator itemEnumerator = project.ProjectItems.GetEnumerator();

            while (itemEnumerator.MoveNext())
            {
                ProjectItem currentItem = (ProjectItem)itemEnumerator.Current;
                items.Add(GetProjectItemsRecursive(currentItem, ref items));
            }

            return items;
        }

        static private ProjectItem GetProjectItemsRecursive(ProjectItem item, ref List<ProjectItem> projectItems)
        {
            if (item.ProjectItems == null)
            {
                return item;
            }

            IEnumerator items = item.ProjectItems.GetEnumerator();

            while (items.MoveNext())
            {
                ProjectItem currentItem = (ProjectItem)items.Current;
                projectItems.Add(GetProjectItemsRecursive(currentItem, ref projectItems));
            }

            return item;
        }
    }
}
