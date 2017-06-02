namespace VCProjectEngineWrapper
{
	public static class VCFileWrapperFactory
	{
		public static IVCFileWrapper create(object wrapped)
		{
			IVCFileWrapper wrapper = null;

			wrapper = new VCFileWrapperVs2017(wrapped);

			if (wrapper == null || !wrapper.isValid())
			{
				wrapper = new VCFileWrapperVs2015(wrapped);
			}
			if (wrapper == null || !wrapper.isValid())
			{
				wrapper = new VCFileWrapperVs2013(wrapped);
			}
			if (wrapper == null || !wrapper.isValid())
			{
				wrapper = new VCFileWrapperVs2012(wrapped);
			}

			return wrapper;
		}
	}
}
