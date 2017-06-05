using System;

namespace VCProjectEngineWrapper
{
	public static class VCProjectWrapperFactory
	{
		public static IVCProjectWrapper create(object wrapped)
		{
			IVCProjectWrapper wrapper = null;

			try
			{
				if (wrapper == null || !wrapper.isValid())
				{
					wrapper = new VCProjectWrapperVs2017(wrapped);
				}
			}
			catch (Exception e)
			{
				wrapper = null;
			}

			try
			{
				if (wrapper == null || !wrapper.isValid())
				{
					wrapper = new VCProjectWrapperVs2015(wrapped);
				}
			}
			catch (Exception e)
			{
				wrapper = null;
			}

			try
			{
				if (wrapper == null || !wrapper.isValid())
				{
					wrapper = new VCProjectWrapperVs2013(wrapped);
				}
			}
			catch (Exception e)
			{
				wrapper = null;
			}

			try
			{
				if (wrapper == null || !wrapper.isValid())
				{
					wrapper = new VCProjectWrapperVs2012(wrapped);
				}
			}
			catch (Exception e)
			{
				wrapper = null;
			}

			return wrapper;
		}
	}
}
