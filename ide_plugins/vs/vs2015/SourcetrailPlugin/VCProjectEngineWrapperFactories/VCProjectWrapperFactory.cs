namespace VCProjectEngineWrapper
{
	public static class VCProjectWrapperFactory
	{
		public static IVCProjectWrapper create(object wrapped)
		{
			IVCProjectWrapper wrapper = null;

			wrapper = new VCProjectWrapperVs2017(wrapped);

			if (wrapper == null || !wrapper.isValid())
			{
				wrapper = new VCProjectWrapperVs2015(wrapped);
			}
			if (wrapper == null || !wrapper.isValid())
			{
				wrapper = new VCProjectWrapperVs2013(wrapped);
			}
			if (wrapper == null || !wrapper.isValid())
			{
				wrapper = new VCProjectWrapperVs2012(wrapped);
			}

			return wrapper;
		}
	}
}
