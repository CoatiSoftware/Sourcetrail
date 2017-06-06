using CoatiSoftware.SourcetrailPlugin.Logging;
using System;
using System.Collections.Generic;

namespace VCProjectEngineWrapper
{
	public static class VCProjectWrapperFactory
	{
		private interface IFactoryModule
		{
			IVCProjectWrapper Create(object wrapped);
		}

		private class FactoryModule2017 : IFactoryModule
		{
			public IVCProjectWrapper Create(object wrapped)
			{
				return new VCProjectWrapperVs2017(wrapped);
			}
		}

		private class FactoryModule2015 : IFactoryModule
		{
			public IVCProjectWrapper Create(object wrapped)
			{
				return new VCProjectWrapperVs2015(wrapped);
			}
		}

		private class FactoryModule2013 : IFactoryModule
		{
			public IVCProjectWrapper Create(object wrapped)
			{
				return new VCProjectWrapperVs2013(wrapped);
			}
		}

		private class FactoryModule2012 : IFactoryModule
		{
			public IVCProjectWrapper Create(object wrapped)
			{
				return new VCProjectWrapperVs2012(wrapped);
			}
		}

		private static Queue<IFactoryModule> modules = null;

		public static IVCProjectWrapper create(object wrapped)
		{
			if (modules == null)
			{
				modules = new Queue<IFactoryModule>();

				// One of these modules will be working for each version of Visual Studio.
				modules.Enqueue(new FactoryModule2017());
				modules.Enqueue(new FactoryModule2015());
				modules.Enqueue(new FactoryModule2013());
				modules.Enqueue(new FactoryModule2012());
			}

			IVCProjectWrapper wrapper = null;
			int testedModuleCount = 0;

			while (wrapper == null && testedModuleCount < modules.Count)
			{
				try
				{
					wrapper = modules.Peek().Create(wrapped);
				}
				catch (Exception e)
				{
					wrapper = null;
				}

				testedModuleCount++;

				if (wrapper == null || !wrapper.isValid())
				{
					// Moving the failing module to the end of the queue.
					// This causes the working module to finall end up in front.
					IFactoryModule failedModule = modules.Dequeue();
					Logging.LogInfo("Discarcing " + failedModule.GetType().Name + " while creating project wrapper.");
					modules.Enqueue(failedModule);
					wrapper = null;
				}
			}

			return wrapper;
		}
	}
}
